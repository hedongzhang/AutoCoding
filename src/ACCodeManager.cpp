/*
 * ACCodeManager.cpp
 *
 *  Created on: 2015年5月14日
 *      Author: HDZhang
 */

#include<fstream>
#include"ACCodeManager.h"
#include"ACManager.h"
using namespace std;
using namespace n_acmanager;

ACCodeManager::ACCodeManager(ACService service):service(service)
{
	cout<<endl<<"=====开始初始化ACCodeManager====="<<endl;
	if(init()==n_acmanager::RETURN_SUCCESS)
	{
		cout<<"=====初始化ACCodeManager完成====="<<endl;
		if(start()==n_acmanager::RETURN_SUCCESS)
		{
			cout<<endl<<"=====生成代码文件成功====="<<endl;
		}
		else
		{
			cout<<endl<<"=====生成代码文件失败====="<<endl;
		}
	}
	else
	{
		cout<<"=====初始化ACCodeManager失败=====";
	}
}

int ACCodeManager::init()
{
	this->headInputPath=this->service.m_KeyValue.find("@HEAD_TEMPLATE_TYPE@")->second;
	this->srcInputPath=this->service.m_KeyValue.find("@SOURC_TEMPLATE_TYPE@")->second;
	if(this->headInputPath.empty() || this->srcInputPath.empty())
	{
		cout<<"无头文件或者源文件模板，无法生成代码文件！"<<endl;
		return n_acmanager::RETURN_FAILED;
	}

	string path=this->service.m_KeyValue.find("@RESULT_PATH@")->second;
	if(path[path.size()-1]!='/')
	{
		this->headOutputPath=path+"/lib64/"+this->service.getOtherMessage().className+".h";
		this->srcOutputPath=path+"/lib64/"+this->service.getOtherMessage().className+".cpp";
	}
	else
	{
		this->headOutputPath=path+"lib64/"+this->service.getOtherMessage().className+".h";
		this->srcOutputPath=path+"lib64/"+this->service.getOtherMessage().className+".cpp";
	}

	string className=this->service.getOtherMessage().className;
	if(className=="" || className.size()<=5)
		return n_acmanager::RETURN_SUCCESS;
	this->coreClassName=className.substr(5);
	this->upperClassName=this->coreClassName;
	for(string::size_type indexTemp=0;indexTemp<this->upperClassName.size();indexTemp++)
	{
		this->upperClassName[indexTemp]=toupper(this->upperClassName[indexTemp]);
	}
	cout<<className<<endl<<"生成：\n"<<this->coreClassName<<endl<<this->upperClassName<<endl;



	return n_acmanager::RETURN_SUCCESS;
}

int ACCodeManager::start()
{
	cout<<"=====开始创建代码文件====="<<endl;
	if(creatHeadFile()!=n_acmanager::RETURN_SUCCESS)
	{
		cout<<"=====创建头文件失败====="<<endl;
		return n_acmanager::RETURN_FAILED;
	}
	if(creatSrcFile()!=n_acmanager::RETURN_SUCCESS)
	{
		cout<<"=====创建源文件失败====="<<endl;
		return n_acmanager::RETURN_FAILED;
	}
	cout<<"=====创建代码完成====="<<endl;
	return n_acmanager::RETURN_SUCCESS;
}

int ACCodeManager::creatHeadFile()
{
	ifstream ifstre;
	ofstream ofstre;

	cout<<this->headInputPath<<endl;
	cout<<this->headOutputPath<<endl;
	ifstre.open(this->headInputPath);
	ofstre.open(this->headOutputPath);
	if(ifstre.is_open() && ofstre.is_open())
	{
		while(!ifstre.eof())
		{
			string currStr;
			getline(ifstre,currStr);

			if(currStr.find('@')!=string::npos)
			{
				currStr=n_acmanager::replaceString(currStr,"@CORE_CLASS_NAME@",this->coreClassName);
				currStr=n_acmanager::replaceString(currStr,"@UPPER_CLASS_NAME@",this->upperClassName);
				currStr=n_acmanager::replaceString(currStr,"@DATE@",this->service.m_KeyValue.find("@DATE@")->second);
				currStr=n_acmanager::replaceString(currStr,"@AUTOR@",this->service.m_KeyValue.find("@AUTOR@")->second);
				currStr=n_acmanager::replaceString(currStr,"@CLASS_ID@",this->service.getBaseMessage().class_id);
				currStr=n_acmanager::replaceString(currStr,"@SERVICE_NAME@",this->service.getBaseMessage().name);

				currStr=n_acmanager::replaceString(currStr,"@INPUT_FIELDS@",creatInputFields());
				currStr=n_acmanager::replaceString(currStr,"@OUTPUT_FIELDS@",creatOutputFields());
			}
			cout<<currStr<<endl;
			ofstre<<currStr<<endl;
		}
		ofstre.flush();
		ofstre.close();
		ifstre.close();
		cout<<"=====生成头文件完成====="<<endl;
	}
	else
	{
		ofstre.close();
		ifstre.close();
		cout<<"=====读取头模板文件或打开头文件生成文件异常====="<<endl;
		return n_acmanager::RETURN_FAILED;
	}

	return n_acmanager::RETURN_SUCCESS;
}

int ACCodeManager::creatSrcFile()
{
	ifstream ifstre;
	ofstream ofstre;

	cout<<this->srcInputPath<<endl;
	cout<<this->srcOutputPath<<endl;
	ifstre.open(this->srcInputPath);
	ofstre.open(this->srcOutputPath);
	if(ifstre.is_open() && ofstre.is_open())
	{
		while(!ifstre.eof())
		{
			string currStr;
			getline(ifstre,currStr);

			if(currStr.find('@')!=string::npos)
			{
				currStr=n_acmanager::replaceString(currStr,"@DATE@",this->service.m_KeyValue.find("@DATE@")->second);
				currStr=n_acmanager::replaceString(currStr,"@AUTOR@",this->service.m_KeyValue.find("@AUTOR@")->second);
				currStr=n_acmanager::replaceString(currStr,"@SERVICE_NAME@",this->service.getBaseMessage().name);
				currStr=n_acmanager::replaceString(currStr,"@CORE_CLASS_NAME@",this->coreClassName);
				currStr=n_acmanager::replaceString(currStr,"@SERVICE_TYPE@",this->service.getOtherMessage().type);

				currStr=n_acmanager::replaceString(currStr,"@GET_INPUT_PARAM@",creatGetInputParam());
				currStr=n_acmanager::replaceString(currStr,"@SET_OUTPUT_PARAM@",creatSetOutputParam());
				currStr=n_acmanager::replaceString(currStr,"@BUSINESSPROCESS@",creatBusinessProcess());
			}
			cout<<currStr<<endl;
			ofstre<<currStr<<endl;
		}
		ofstre.flush();
		ofstre.close();
		ifstre.close();
		cout<<"=====生成源文件完成====="<<endl;
	}
	else
	{
		ofstre.close();
		ifstre.close();
		cout<<"=====读取源模板文件或打开源文件生成文件异常====="<<endl;
		return n_acmanager::RETURN_FAILED;
	}

	return n_acmanager::RETURN_SUCCESS;
}

string ACCodeManager::creatInputFields()
{
	string inputFields="";
#if 0
	for(vector<string>::size_type currIndex=0;currIndex<this->service.getNumInoutBindParam();currIndex++)
	{
		if(this->service.getInputField()[currIndex].type==FieldType::STRING)
		{
			inputFields+="\t\tchar "+this->service.getInputField()[currIndex].name+"["+n_acmanager::longToString(this->service.getInputField()[currIndex].size)+"];\n";
		}
		else if(this->service.getInputField()[currIndex].type==FieldType::LONG)
		{
			inputFields+="\t\tlong "+this->service.getInputField()[currIndex].name+";\n";
		}
		else
		{

		}

	}
#endif
	for(Field currField:this->service.getInputField())
	{
		if(currField.type==FieldType::STRING)
		{
			inputFields+="\t\tchar "+currField.name+"["+n_acmanager::longToString(currField.size)+"];\n";
		}
		else if(currField.type==FieldType::LONG)
		{
			inputFields+="\t\tlong "+currField.name+";\n";
		}
		else
		{

		}

	}
	return inputFields;
}

string ACCodeManager::creatOutputFields()
{
	string outputFields="";
	for(vector<string>::size_type currIndex=0;currIndex<this->service.getNumOutputBindParam();currIndex++)
	{
		if(this->service.getOutputField()[currIndex].type==FieldType::STRING)
		{
			outputFields+="\t\tchar "+this->service.getOutputField()[currIndex].name+"["+n_acmanager::longToString(this->service.getOutputField()[currIndex].size)+"];\n";
		}
		else if(this->service.getOutputField()[currIndex].type==FieldType::LONG)
		{
			outputFields+="\t\tlong "+this->service.getOutputField()[currIndex].name+";\n";
		}
		else
		{

		}

	}
	return outputFields;
}


string ACCodeManager::creatGetInputParam()
{
	string getInputParamStr="";
	this->AssemblyHead="";
	this->AssemblyEnd="";

	if(this->service.getBaseMessage().input_node=="NULL")
	{
		getInputParamStr+="\n\tDCTuxBuffer *pTuxBuffer = (DCTuxBuffer*)pBuffer;\n";
		getInputParamStr+="\tST@CORE_CLASS_NAME@In st@CORE_CLASS_NAME@In;\n";
		getInputParamStr+="\tmemset(&st@CORE_CLASS_NAME@In,0x00,sizeof(st@CORE_CLASS_NAME@In));\n";

		string allField="";
		string allAssemblyHeadTemp="";
		for(Field currField:this->service.getInputField())
		{
			string getParamTmpl="";
			string AssemblyHeadTemp="";
			if(currField.type==FieldType::STRING)
			{
				if(currField.isNihil)
				{
					getParamTmpl=paramTmplStringNihil;
				}
				else if(currField.needAssembly)
				{
					getParamTmpl=paramTmplStringAssembly;
					if(this->service.getOtherMessage().type==ACServiceTypeSelect)
					{
						AssemblyHeadTemp=AssemblyHeadTmplSelectStr;
					}
					else
					{
						AssemblyHeadTemp=AssemblyHeadTmplUpdateStr;
					}
					if(this->AssemblyEnd=="")
					{
						this->AssemblyEnd=AssemblyEndTmpl;
					}
				}
				else
				{
					getParamTmpl=paramTmplString;
				}
			}
			else if(currField.type==FieldType::LONG)
			{
				if(currField.needAssembly)
				{
					getParamTmpl=paramTmplLongAssembly;
					if(this->service.getOtherMessage().type==ACServiceTypeSelect)
					{
						AssemblyHeadTemp=AssemblyHeadTmplSelectLong;
					}
					else
					{
						AssemblyHeadTemp=AssemblyHeadTmplUpdateLong;
					}
					if(AssemblyEnd=="")
					{
						AssemblyEnd=AssemblyEndTmpl;
					}
				}
				else
				{
					getParamTmpl=paramTmplLong;
				}
			}
			getParamTmpl=n_acmanager::replaceString(getParamTmpl,"@FIELD_NAME@",currField.name);
			getParamTmpl=n_acmanager::replaceString(getParamTmpl,"@CORE_CLASS_NAME@",this->coreClassName);
			AssemblyHeadTemp=n_acmanager::replaceString(AssemblyHeadTemp,"@FIELD_NAME@",currField.name);
			allAssemblyHeadTemp+=AssemblyHeadTemp;
			if(currField.name=="LATN_ID")
			{
				allField=getParamTmpl+allField;
			}
			else
			{
				allField+=getParamTmpl;
			}
		}
		this->AssemblyHead+=allAssemblyHeadTemp;
		getInputParamStr+=allField;
		getInputParamStr+="\tthis->l_@CORE_CLASS_NAME@In.push_back(st@CORE_CLASS_NAME@In);\n";
		getInputParamStr=n_acmanager::replaceString(getInputParamStr,"@CORE_CLASS_NAME@",this->coreClassName);
	}
	else
	{
		getInputParamStr+="\n\tDCTuxBuffer *pTuxBufferAll = (DCTuxBuffer*)pBuffer;\n";
		getInputParamStr+=inputTempHead;
		getInputParamStr+="\t\tST@CORE_CLASS_NAME@In st@CORE_CLASS_NAME@In;\n";
		getInputParamStr+="\t\tmemset(&st@CORE_CLASS_NAME@In,0x00,sizeof(st@CORE_CLASS_NAME@In));\n";

		string allField="";
		string allAssemblyHeadTemp="";
		for(Field currField:this->service.getInputField())
		{
			string getParamTmpl="";
			string AssemblyHeadTemp="";
			if(currField.type==FieldType::STRING)
			{
				if(currField.isNihil)
				{
					getParamTmpl=paramTmplStringNihil;
				}
				else if(currField.needAssembly)
				{
					getParamTmpl=paramTmplStringAssembly;
					if(this->service.getOtherMessage().type==ACServiceTypeSelect)
					{
						AssemblyHeadTemp=AssemblyHeadTmplSelectStr;
					}
					else
					{
						AssemblyHeadTemp=AssemblyHeadTmplUpdateStr;
					}
					if(this->AssemblyEnd=="")
					{
						this->AssemblyEnd=AssemblyEndTmpl;
					}
				}
				else
				{
					getParamTmpl=paramTmplString;
				}
			}
			else if(currField.type==FieldType::LONG)
			{
				if(currField.needAssembly)
				{
					getParamTmpl=paramTmplLongAssembly;
					if(this->service.getOtherMessage().type==ACServiceTypeSelect)
					{
						AssemblyHeadTemp=AssemblyHeadTmplSelectLong;
					}
					else
					{
						AssemblyHeadTemp=AssemblyHeadTmplUpdateLong;
					}
					if(this->AssemblyEnd=="")
					{
						this->AssemblyEnd=AssemblyEndTmpl;
					}
				}
				else
				{
					getParamTmpl=paramTmplLong;
				}
			}
			getParamTmpl=n_acmanager::replaceString(getParamTmpl,"@FIELD_NAME@",currField.name);
			getParamTmpl=n_acmanager::replaceString(getParamTmpl,"@CORE_CLASS_NAME@",this->coreClassName);
			AssemblyHeadTemp=n_acmanager::replaceString(AssemblyHeadTemp,"@FIELD_NAME@",currField.name);
			allAssemblyHeadTemp+=AssemblyHeadTemp;

			if(currField.name=="LATN_ID")
			{
				allField=getParamTmpl+allField;
			}
			else
			{
				allField+=getParamTmpl;
			}
		}
		this->AssemblyHead+=allAssemblyHeadTemp;
		getInputParamStr+=allField;
		getInputParamStr+="\t\tthis->l_@CORE_CLASS_NAME@In.push_back(st@CORE_CLASS_NAME@In);\n";
		getInputParamStr=n_acmanager::replaceString(getInputParamStr,"@CORE_CLASS_NAME@",this->coreClassName);
		getInputParamStr=n_acmanager::replaceString(getInputParamStr,"@NODE_NAME@",this->service.getBaseMessage().input_node);
		getInputParamStr+="\t}\n";
	}
	return getInputParamStr;
}

string ACCodeManager::creatBusinessProcess()
{
	string bissnessProcessStr=bissnessProcessHead;
	string latnID="0";
	for(Field currField:this->service.getInputField())
	{
		if(currField.name=="LATN_ID")
		{
			latnID="iter->LATN_ID";
		}
	}
	bissnessProcessStr=n_acmanager::replaceString(bissnessProcessStr,"@LATN_ID@",latnID);

	/*if(this->service.getOtherMessage().type==ACServiceTypeSelect)
	{
		bissnessProcessStr+=bissnessProcessEnd1;
	}
	else
	{
		bissnessProcessStr+=bissnessProcessEnd2;
	}*/
	if(this->service.getNumOutputBindParam()!=0)
	{
		bissnessProcessStr+=bissnessProcessEnd1;
	}
	else
	{
		bissnessProcessStr+=bissnessProcessEnd2;
	}


	bissnessProcessStr=n_acmanager::replaceString(bissnessProcessStr,"@SERVICE_NAME@",this->service.getBaseMessage().name);
	bissnessProcessStr=n_acmanager::replaceString(bissnessProcessStr,"@CORE_CLASS_NAME@",this->coreClassName);
	bissnessProcessStr=n_acmanager::replaceString(bissnessProcessStr,"@ASSEMBLY_HEAD@",this->AssemblyHead);
	bissnessProcessStr=n_acmanager::replaceString(bissnessProcessStr,"@ASSEMBLY_END@",this->AssemblyEnd);

	return bissnessProcessStr;
}

string ACCodeManager::creatSetOutputParam()
{
	string setOutputParamStr="";
	if(this->service.getOutputField().size()==0)
		return setOutputParamStr;

	setOutputParamStr=+"\tDCTuxBuffer *pTuxBuffer = (DCTuxBuffer*)pBuffer;\n";
	if(this->service.getBaseMessage().output_node=="NULL")
	{
		setOutputParamStr+="\tlist<ST@CORE_CLASS_NAME@Out>::iterator iter=l_@CORE_CLASS_NAME@Out.begin();\n\t";

		setOutputParamStr+="pTuxBuffer->RsOpen();\n";

		for(Field currField:this->service.getOutputField())
		{
			string setParamTmpl="";
			if(currField.type==FieldType::STRING)
			{
				setParamTmpl=outParamTmplString1;
			}
			else if(currField.type==FieldType::LONG)
			{
				setParamTmpl=outParamTmplLong1;
			}
			setParamTmpl=n_acmanager::replaceString(setParamTmpl,"@FIELD_NAME@",currField.name);
			//getParamTmpl=n_acmanager::replaceString(getParamTmpl,"@CORE_CLASS_NAME@",this->coreClassName);
			setOutputParamStr+=setParamTmpl;
		}
		//setOutputParamStr+="\tthis->l_@CORE_CLASS_NAME@In.push_back(st@CORE_CLASS_NAME@In)\n";
		setOutputParamStr=n_acmanager::replaceString(setOutputParamStr,"@CORE_CLASS_NAME@",this->coreClassName);
	}
	else
	{
		setOutputParamStr+=outputTempHead;
		for(Field currField:this->service.getOutputField())
		{
			string setParamTmpl="";
			if(currField.type==FieldType::STRING)
			{
				setParamTmpl="\t\t"+outParamTmplString2;
			}
			else if(currField.type==FieldType::LONG)
			{
				setParamTmpl="\t\t"+outParamTmplLong2;
			}
			setParamTmpl=n_acmanager::replaceString(setParamTmpl,"@FIELD_NAME@",currField.name);
			//setParamTmpl=n_acmanager::replaceString(getParamTmpl,"@CORE_CLASS_NAME@",this->coreClassName);
			setOutputParamStr+=setParamTmpl;
		}
		setOutputParamStr+=outputTempEnd;
		setOutputParamStr=n_acmanager::replaceString(setOutputParamStr,"@CORE_CLASS_NAME@",this->coreClassName);
		setOutputParamStr=n_acmanager::replaceString(setOutputParamStr,"@NODE_NAME@",this->service.getBaseMessage().output_node);

	}
	return setOutputParamStr;
}
