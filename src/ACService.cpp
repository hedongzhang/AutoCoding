/*
 * ACService.cpp
 *
 *  Created on: 2015年5月5日
 *      Author: HDZhang
 */

#include"ACService.h"
#include"ACManager.h"
#include"ACCodeManager.h"
#include"ACSQLManager.h"
#include"ACConfManager.h"
#include<fstream>
#include<sstream>

using namespace n_acmanager;

ACService::ACService(map<string,string>& baseMessage):m_KeyValue(baseMessage)
{
	inputFields.clear();
	outputFields.clear();
	//m_KeyValue.clear();

	cout<<endl<<"=====开始创建服务=====<"<<this->m_KeyValue.find("@SERVICE_NAME@")->second<<">"<<endl;

	if(init()!=RETURN_SUCCESS)
	{
		cout<<"=====ACService初始化失败=====<"<<this->baseMess.name<<">"<<endl;
	}
}

ACService::~ACService()
{
}


/**********初始化服务**********
	 *
	 */
int ACService::init()
{
	//提取服务基本信息
	this->baseMess.name=this->m_KeyValue.find("@SERVICE_NAME@")->second;
	this->baseMess.sql=this->m_KeyValue.find("@SQL@")->second;
	this->baseMess.class_id=this->m_KeyValue.find("@CLASS_ID@")->second;
	this->baseMess.group_id=this->m_KeyValue.find("@GROUP_ID@")->second;
	this->baseMess.input_node=this->m_KeyValue.find("@INPUT_NODE@")->second;
	this->baseMess.output_node=this->m_KeyValue.find("@OUTPUT_NODE@")->second;
	this->baseMess.libName=this->m_KeyValue.find("@LIB_NAME@")->second;
	//this->baseMess.type=this->m_KeyValue.find("@SERVICE_TYPE@")->second;

	//初始化生成其他服务信息
	if(initOtherMessage()!=n_acmanager::RETURN_SUCCESS)
	{
		return n_acmanager::RETURN_FAILED;
	}

	//生成输入输出字段信息
	if(this->m_KeyValue.find("@FIELDS@")!=m_KeyValue.end())
	{
		string fieldFilePath=this->m_KeyValue.find("@FIELDS@")->second;
		if(readFieldsFile(fieldFilePath)!=n_acmanager::RETURN_SUCCESS)
		{
			return n_acmanager::RETURN_FAILED;
		}
	}
	else
	{
		if(initInputFields()==n_acmanager::RETURN_FAILED || initOutputFields()==n_acmanager::RETURN_FAILED)
		{
			cout<<"=====根据sql初始化输入输出字段失败！====="<<endl;
			return n_acmanager::RETURN_FAILED;
		}
	}

	//处理配置文件
	ACConfManager acConfManager(*this);
	//生成sql脚本
	ACSQLManager acSQLManager(*this);

	//打印服务信息
	//printService();
	ACCodeManager acCodeManager(*this);
	//ACCodeManager acCodeManager("");

	return n_acmanager::RETURN_SUCCESS;
}

int ACService::initOtherMessage()
{
	if(creatClassName(this->otherMess.className)!=n_acmanager::RETURN_SUCCESS)
		return RETURN_FAILED;
	if(creatServiceType()!=n_acmanager::RETURN_SUCCESS)
		return RETURN_FAILED;
	return RETURN_SUCCESS;
}

int ACService::creatClassName(string& className)
{
	if(this->baseMess.name=="")
	{
		cout<<"服务名为空，生成类型名失败！"<<endl;
		return n_acmanager::RETURN_FAILED;
	}

	string serviceName=this->baseMess.name;
	toupper(serviceName[0]);
	string::size_type pos = 1;
	while (pos < serviceName.length())
	{
		if (serviceName[pos] == '_') {
			serviceName.erase(pos, 1);
			serviceName[pos] = toupper(serviceName[pos]);
		} else {
			serviceName[pos] = tolower(serviceName[pos]);
		}
		pos++;
	}
	className = "DCCrm";
	className += serviceName;

	//cout << "生成类名：" + className << endl;
	return n_acmanager::RETURN_SUCCESS;
}
int ACService::creatServiceType()
{
	if(m_KeyValue.find("@SERVICE_TYPE@")!=m_KeyValue.end())
	{
		this->otherMess.type=this->m_KeyValue.find("@SERVICE_TYPE@")->second;
		return RETURN_SUCCESS;
	}
	else
	{
		string::size_type currIndex=0;
		while(  baseMess.sql[currIndex]==' ' || baseMess.sql[currIndex]=='\t' || \
				baseMess.sql[currIndex]=='\v' || baseMess.sql[currIndex]=='\n' || \
				baseMess.sql[currIndex]=='\r' )
			currIndex++;

		if(	this->baseMess.sql[currIndex]=='s' || this->baseMess.sql[currIndex]=='S' )
		{
			this->otherMess.type=ACServiceTypeSelect;
		}
		else if( this->baseMess.sql[currIndex]=='u' || this->baseMess.sql[currIndex]=='U' )
		{
			this->otherMess.type=ACServiceTypeUpdate;
		}
		else if( this->baseMess.sql[currIndex]=='d' || this->baseMess.sql[currIndex]=='D' )
		{
			this->otherMess.type=ACServiceTypeDelete;
		}
		else if( this->baseMess.sql[currIndex]=='i' || this->baseMess.sql[currIndex]=='I' )
		{
			this->otherMess.type=ACServiceTypeInsert;
		}
		else if( this->baseMess.sql[currIndex]=='b' || this->baseMess.sql[currIndex]=='B' )
		{
			this->otherMess.type=ACServiceTypeCombo;
		}
		else
		{
			return n_acmanager::RETURN_FAILED;
		}
		return n_acmanager::RETURN_SUCCESS;
	}
}

/*根据sql初始化输入字段
 * 读取每个[:]之前直到空格的作为输入字段名称（自动转换为大写）
 * exp:
 * 		latn_id = :LATN_ID
   	   	and a.acc_nbr = :ACC_NBR
        其他都会忽略
   exp:
   	   	to_char(a.record_time, 'yyyy-mm-dd hh:mi:ss') data ,
 */
int ACService::initInputFields()
{
	cout<<endl<<"=====根据sql初始化输入字段====="<<endl;
	if(this->baseMess.sql=="")
	{
		cout<<"服务sql为空，无法生成输入字段!"<<endl;
		return n_acmanager::RETURN_FAILED;
	}
	string sql=this->baseMess.sql;
	string::size_type currPos = 0;
	string::size_type fieldPos = 0;
	while (currPos < sql.length())
	{
		if(fieldPos==0 && sql[currPos]==':')
		{
			fieldPos=currPos+1;
		}
		else if(fieldPos!=0 && sql[currPos]==':')
		{
			fieldPos=0;
		}
		else if( fieldPos!=0 && sql[currPos]==' ')
		{
			for(string::size_type i=fieldPos;i<currPos;i++)
			{
				sql[i]=toupper(sql[i]);
			}

			Field field;
			field.type=FieldType::STRING;
			field.name=sql.substr(fieldPos,currPos-fieldPos);
			field.size=64;
			this->inputFields.push_back(field);
			fieldPos=0;
		}

		currPos++;
	}
	this->numInputBindParam=this->inputFields.size();

	return n_acmanager::RETURN_SUCCESS;
}

/*根据sql初始化输出字段
 * 读取每个[,]和空格之前直到另外一个空格为止的作为输出字段名称（自动转换为大写）
 * exp:
 *		select a.latn_id latn_id ,
 *		a.acc_nbr acc_nbr ,
 * 其他都会忽略
 * exp:
 * 	   	to_char(a.record_time, 'yyyy-mm-dd hh:mi:ss') data
 * 	   	a.pdf_path pdf_path,\
 * 	   	[,]之前无空格
 */
int ACService::initOutputFields()
{
	cout<<endl<<"=====根据sql初始化输出字段====="<<endl;
	if(this->baseMess.sql=="")
	{
		cout<<"=====服务sql为空，无法生成输出字段!====="<<endl;
		return n_acmanager::RETURN_FAILED;
	}
	string sql=this->baseMess.sql;

	string::size_type currIndex=0;
	while(  baseMess.sql[currIndex]==' ' || baseMess.sql[currIndex]=='\t' || \
			baseMess.sql[currIndex]=='\v' || baseMess.sql[currIndex]=='\n' || \
			baseMess.sql[currIndex]=='\r' )
		currIndex++;

	if(sql[currIndex]!='s' && sql[currIndex]!='S')
	{
		//cout<<"=====不是select语句，无输出字段！====="<<endl;
		return n_acmanager::RETURN_SUCCESS;
	}




	string::size_type currPos = 0;
	while(currPos!=sql.size())
	{
		if(sql[currPos]==',' && sql[currPos-1]==' ')
		{
			string::size_type size=2;
			while(sql[currPos-size]!=' ')
			{
				size++;
			}

			Field field;
			field.name=sql.substr(currPos-size+1,size-2);

			for(string::size_type i=0;i<field.name.size();i++)
			{
				field.name[i]=toupper(field.name[i]);
			}
			sql.substr(currPos-size+1,size-2);
			field.size=64;
			field.type=FieldType::STRING;
			this->outputFields.push_back(field);
		}
		currPos++;
	}
	this->numOutputBindParam=this->outputFields.size();

	return n_acmanager::RETURN_SUCCESS;

}

/**********从文件读取输入输出字段**********
 *
 */
int ACService::readFieldsFile(string path)
{
	cout<<endl<<"=====开始读取字段配置文件====="<<endl;
	ifstream ifstre;
	ifstre.open(path);
	if(ifstre.is_open())
	{
		string currFieldType="";
		string currLineValue="";
		this->numInputBindParam=0;
		this->numOutputBindParam=0;
		while(!ifstre.eof())
		{
			getline(ifstre,currLineValue);

			if(currLineValue[0]=='@')
			{
				istringstream istringstre(currLineValue);
				istringstre>>currFieldType;
				if(currFieldType=="@INPUT_BIND_FIELD@")
					istringstre>>this->numInputBindParam;
				else if(currFieldType=="@OUTPUT_BIND_FIELD@")
					istringstre>>this->numOutputBindParam;
			}
			else if(currFieldType.empty() || currLineValue.empty() || currLineValue[0]=='#')
			{
				continue;
			}
			else if(currFieldType=="@INPUT_BIND_FIELD@")
			{
				for(string::size_type currIndex=0;currIndex<currLineValue.length();currIndex++)
					currLineValue[currIndex]=toupper(currLineValue[currIndex]);

				istringstream istringstre(currLineValue);

				Field field;
				field.size=0;
				istringstre>>field.name;

				string temp;
				istringstre>>temp;
				if(temp!="")
				{
					if(temp=="$")
					{
						field.isNihil=true;
					}
					else if(temp=="%")
					{
						field.needAssembly=true;
					}
					else
					{

						istringstream istringstreTemp(temp);
						istringstreTemp>>field.size;
						istringstre>>temp;
						if(temp!="")
						{
							if(temp=="$")
							{
								field.isNihil=true;
							}
							else if(temp=="%")
							{
								field.needAssembly=true;
							}
						}
					}
				}

				if(field.size==0)
					field.size=64;
				field.type=FieldType::STRING;
				this->inputFields.push_back(field);

			}
			else if(currFieldType=="@OUTPUT_BIND_FIELD@")
			{
				for(string::size_type currIndex=0;currIndex<currLineValue.length();currIndex++)
					currLineValue[currIndex]=toupper(currLineValue[currIndex]);

				istringstream istringstre(currLineValue);

				Field field;
				field.size=0;
				istringstre>>field.name;
				istringstre>>field.size;
				if(field.size==0)
					field.size=64;
				field.type=FieldType::STRING;
				this->outputFields.push_back(field);
			}
		}
		/*if(this->numInputBindParam==0)
			this->numInputBindParam=this->inputFields.size();
		if(this->numOutputBindParam==0)
			this->numOutputBindParam=this->outputFields.size();*/
		cout<<"======读取字段配置完成====="<<endl<<endl;
		ifstre.close();
	}
	else
	{
		cout<<"=====读取字段配置文件异常====="<<endl<<endl;
		ifstre.close();
		return RETURN_FAILED;
	}
	return n_acmanager::RETURN_SUCCESS;
}


//打印服务信息
int ACService::printService()
{
	cout<<"================打印服务================"<<endl;
	cout<<"基本信息："<<endl<<"{"<<endl;
	cout<<"	服务名："<<this->baseMess.name<<endl;
	cout<<"	class_id："<<this->baseMess.class_id<<endl;
	cout<<"	group_id："<<this->baseMess.group_id<<endl;
	cout<<"	入参节点："<<this->baseMess.input_node<<endl;
	cout<<"	出参节点："<<this->baseMess.output_node<<endl;
	cout<<"	库名："<<this->baseMess.libName<<endl;
	cout<<"	SQL："<<this->baseMess.sql<<endl;
	cout<<"}"<<endl;
	cout<<"生成信息："<<endl<<"{"<<endl;
	cout<<"	生成类名："<<this->otherMess.className<<endl;
	cout<<"	生成服务类型："<<this->otherMess.type<<endl;
	cout<<"}"<<endl<<endl;



	cout<<"输入字段（"<<this->inputFields.size()<<"）："<<endl;
	cout<<"名称";
	for(int i=0;i<68;i++)
	{
		cout<<" ";
	}
	cout<<"类型            大小            虚值"<<endl;
	for(Field field:this->inputFields)
	{
		int spaceNum=32-field.name.length();
		cout<<field.name;
		for(int i=0;i<spaceNum;i++)
		{
			cout<<" ";
		}
		cout<<field.type<<"      "<<field.size<<"      "<<field.isNihil<<endl;
	}
	cout<<endl<<"输出字段（"<<this->outputFields.size()<<"）："<<endl;
	cout<<"名称";
	for(int i=0;i<68;i++)
	{
		cout<<" ";
	}
	cout<<"类型            大小            虚值"<<endl;
	for(Field field:this->outputFields)
	{
		int spaceNum=32-field.name.length();
		cout<<field.name;
		for(int i=0;i<spaceNum;i++)
		{
			cout<<" ";
		}
		cout<<field.type<<"      "<<field.size<<"      "<<field.isNihil<<endl;
	}
	cout<<"======================================="<<endl;
	return n_acmanager::RETURN_SUCCESS;
}

ACService::BaseMess& ACService::getBaseMessage()
{
	return this->baseMess;
}

ACService::OtherMess& ACService::getOtherMessage()
{
	return this->otherMess;
}

//访问输入输出字段
int ACService::setInputField(vector<Field>& field)
{
	this->inputFields=field;
	return n_acmanager::RETURN_SUCCESS;
}
vector<Field>& ACService::getInputField()
{
	return this->inputFields;
}
int ACService::setOutputField(vector<Field>& field)
{
	this->outputFields=field;
	return n_acmanager::RETURN_SUCCESS;
}
vector<Field>& ACService::getOutputField()
{
	return this->outputFields;
}

//获取SQL绑定参数个数
int ACService::getNumInoutBindParam()
{
	return this->numInputBindParam;
}
//获取SQL返回参数个数
int ACService::getNumOutputBindParam()
{
	return this->numOutputBindParam;
}
