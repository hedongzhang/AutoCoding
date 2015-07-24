/*
 * ACConfManager.cpp
 *
 *  Created on: 2015年5月4日
 *      Author: HDZhang
 */

#include<iostream>
#include<fstream>
#include<memory.h>
#include<sstream>
#include"ACConfManager.h"

using namespace n_acmanager;

ACConfManager::ACConfManager(ACService& service)
{
	this->maxFieldNum=0;
	string path=service.m_KeyValue.find("@RESULT_PATH@")->second;
	if(path[path.size()-1]!='/')
	{
		this->crmdemFileName=path+"/bin/crmdem.fml32";
		this->metadataFileName=path+"/bin/metadata.in";
	}
	else
	{
		this->crmdemFileName=path+"bin/crmdem.fml32";
		this->metadataFileName=path+"bin/metadata.in";
	}
	complateConfig(service);
}

int ACConfManager::readCrmdemFml32()
{
	ifstream ifstre;
	ifstre.open(this->crmdemFileName);
	if(ifstre.is_open())
	{
		while(!ifstre.eof())
		{
			string currStr;
			getline(ifstre,currStr);
			if(currStr=="" || currStr=="\n")
				continue;
			istringstream istringStre(currStr);

			Field currFieldMess;
			istringStre>>currFieldMess.name>>currFieldMess.size;
			string currFieldType={0};
			istringStre>>currFieldType;
			if(currFieldType=="long")
				currFieldMess.type=FieldType::LONG;
			else if(currFieldType=="string")
				currFieldMess.type=FieldType::STRING;
			else if(currFieldType=="fml32")
				currFieldMess.type=FieldType::FML32;

			this->m_allFileMess.insert(pair<string,Field>(currFieldMess.name,currFieldMess));
			if(currFieldMess.size>maxFieldNum)
			{
				this->maxFieldNum=currFieldMess.size;
			}
			//cout<<currStr<<endl;
		}
		cout<<"=====读取crmdem.fml32文件完成====="<<endl;
		cout<<"读取数据"<<m_allFileMess.size()<<"条！"<<endl;
		ifstre.close();
	}
	else
	{
		cout<<"=====读取crmdem.fml32文件异常====="<<endl;
		ifstre.close();
		return n_acmanager::RETURN_FAILED;
	}

	return n_acmanager::RETURN_SUCCESS;
}

int ACConfManager::readMetadataIn()
{
	ifstream ifstre;
	ifstre.open(this->metadataFileName);
	if(ifstre.is_open())
	{
		while(!ifstre.eof())
		{
			string currStr;
			getline(ifstre,currStr);
			if(currStr.substr(0,8)=="service=")
			{
				s_services.insert(currStr.substr(8,currStr.size()-8));
				//cout<<currStr.substr(8,currStr.size()-8)<<endl;
			}
		}
		cout<<"=====读取metadata.in文件完成====="<<endl;
		cout<<"读取数据"<<s_services.size()<<"个服务！"<<endl;
		ifstre.close();
	}
	else
	{
		cout<<"=====读取metadata.in文件异常====="<<endl;
		ifstre.close();
		return n_acmanager::RETURN_FAILED;
	}

	return n_acmanager::RETURN_SUCCESS;
}
/* 处理配置文件
 *
 */
int ACConfManager::complateConfig(ACService& service)
{

	if(readCrmdemFml32()!=n_acmanager::RETURN_SUCCESS)
	{
		return n_acmanager::RETURN_FAILED;
	}
	if(readMetadataIn()!=n_acmanager::RETURN_SUCCESS)
	{
		return n_acmanager::RETURN_FAILED;
	}
	if(parseCrmdemFml32(service)!=n_acmanager::RETURN_SUCCESS)
	{
		return n_acmanager::RETURN_FAILED;
	}
	return parseMetadataIn(service);
}

int ACConfManager::parseCrmdemFml32(ACService& service)
{
	cout<<"=====入参===="<<endl;
	vector<Field>& v_inputFields=service.getInputField();
	for(string::size_type i=0;i<v_inputFields.size();i++)
	{
		if(this->m_allFileMess.find(v_inputFields[i].name)!= this->m_allFileMess.end())
		{
			v_inputFields[i].type=this->m_allFileMess.find(v_inputFields[i].name)->second.type;
			//v_inputFields[i]=(this->m_allFileMess.find(v_inputFields[i].name))->second;

			if(v_inputFields[i].type==FieldType::STRING)
			{
				if(v_inputFields[i].size==0)
					v_inputFields[i].size=64;
			}
			else if(v_inputFields[i].type==FieldType::LONG && !v_inputFields[i].isNihil)
			{
				v_inputFields[i].size=0;
			}
			else if(v_inputFields[i].type==FieldType::FML32 || v_inputFields[i].isNihil)
			{
				v_inputFields[i].name=v_inputFields[i].name+"_NEW";
				i--;
				continue;
			}
			cout<<"----"<<v_inputFields[i].name<<"	"<<v_inputFields[i].size<<"	"<<v_inputFields[i].type<<"----"<<endl;
		}
		else
		{
			if(v_inputFields[i].size==0)
				v_inputFields[i].size=64;
			v_inputFields[i].type=FieldType::STRING;
			if(writeCrmdemFml32(v_inputFields[i])!=n_acmanager::RETURN_SUCCESS)
			{
				return n_acmanager::RETURN_FAILED;
			}
			this->m_allFileMess.insert(pair<string,Field>(v_inputFields[i].name,v_inputFields[i]));
			cout<<"****"<<v_inputFields[i].name<<"	"<<this->maxFieldNum<<"	"<<v_inputFields[i].type<<"****"<<endl;
		}
	}

	cout<<"=====出参===="<<endl;
	vector<Field>& v_outputFields=service.getOutputField();
	for(string::size_type i=0;i<v_outputFields.size();i++)
	{
		if(this->m_allFileMess.find(v_outputFields[i].name)!= this->m_allFileMess.end())
		{
			v_outputFields[i].type=this->m_allFileMess.find(v_outputFields[i].name)->second.type;
			//v_outputFields[i]=(this->m_allFileMess.find(v_outputFields[i].name))->second;
			if(v_outputFields[i].type==FieldType::LONG)
				v_outputFields[i].size=0;
			else if(v_outputFields[i].type==FieldType::STRING)
			{
				if(v_outputFields[i].size==0)
					v_outputFields[i].size=64;
			}
			else if(v_outputFields[i].type==FieldType::FML32)
			{
				v_outputFields[i].name=v_outputFields[i].name+"_NEW";
				i--;
				continue;
			}
			cout<<"----"<<v_outputFields[i].name<<"	"<<v_outputFields[i].size<<"	"<<v_outputFields[i].type<<"----"<<endl;
		}
		else
		{
			if(v_outputFields[i].size==0)
				v_outputFields[i].size=64;
			v_outputFields[i].type=FieldType::STRING;
			if(writeCrmdemFml32(v_outputFields[i])!=n_acmanager::RETURN_SUCCESS)
			{
				return n_acmanager::RETURN_FAILED;
			}
			this->m_allFileMess.insert(pair<string,Field>(v_outputFields[i].name,v_outputFields[i]));
			cout<<"****"<<v_outputFields[i].name<<"	"<<this->maxFieldNum<<"	"<<v_outputFields[i].type<<"****"<<endl;
		}

	}

	if(service.getBaseMessage().input_node!="NULL")
	{
		Field inputField;
		inputField.name=service.getBaseMessage().input_node;
		inputField.type=FieldType::FML32;
		inputField.size=0;

		cout<<"=====入参节点===="<<endl;
		if(this->m_allFileMess.find(inputField.name)== this->m_allFileMess.end())
		{
			if(writeCrmdemFml32(inputField)!=n_acmanager::RETURN_SUCCESS)
			{
				return n_acmanager::RETURN_FAILED;
			}
			this->m_allFileMess.insert(pair<string,Field>(inputField.name,inputField));
			service.getInputField().push_back(inputField);
			cout<<"****"<<inputField.name<<"	"<<this->maxFieldNum<<"	"<<inputField.type<<"****"<<endl;
		}
		else
		{
			Field& inputFieldSrc=this->m_allFileMess.find(inputField.name)->second;
			if(inputFieldSrc.type==FieldType::LONG || inputFieldSrc.type==FieldType::STRING)
			{
				inputField.name=inputField.name+"_NEW";
				service.getBaseMessage().input_node=inputField.name;
				if(this->m_allFileMess.find(inputField.name)== this->m_allFileMess.end())
				{
					if(writeCrmdemFml32(inputField)!=n_acmanager::RETURN_SUCCESS)
					{
						return n_acmanager::RETURN_FAILED;
					}
					cout<<"****"<<inputField.name<<"	"<<this->maxFieldNum<<"	"<<inputField.type<<"****"<<endl;
				}
			}
			this->m_allFileMess.insert(pair<string,Field>(inputField.name,inputField));
			service.getInputField().push_back(inputField);
			cout<<"----"<<inputField.name<<"	"<<inputField.size<<"	"<<inputField.type<<"----"<<endl;
		}
	}
	if(service.getBaseMessage().output_node!="NULL")
	{
		Field outputField;
		outputField.name=service.getBaseMessage().output_node;
		outputField.type=FieldType::FML32;
		outputField.size=0;

		cout<<"=====出参节点===="<<endl;
		if(this->m_allFileMess.find(outputField.name)== this->m_allFileMess.end())
		{
			if(writeCrmdemFml32(outputField)!=n_acmanager::RETURN_SUCCESS)
			{
				return n_acmanager::RETURN_FAILED;
			}
			this->m_allFileMess.insert(pair<string,Field>(outputField.name,outputField));
			service.getOutputField().push_back(outputField);
			cout<<"****"<<outputField.name<<"	"<<this->maxFieldNum<<"	"<<outputField.type<<"****"<<endl;
		}
		else
		{
			Field& outputFieldSrc=this->m_allFileMess.find(outputField.name)->second;
			if(outputFieldSrc.type==FieldType::LONG || outputFieldSrc.type==FieldType::STRING)
			{
				outputField.name=outputField.name+"_NEW";
				service.getBaseMessage().output_node=outputField.name;
				if(this->m_allFileMess.find(outputField.name)== this->m_allFileMess.end())
				{
					if(writeCrmdemFml32(outputField)!=n_acmanager::RETURN_SUCCESS)
					{
						return n_acmanager::RETURN_FAILED;
					}
					cout<<"****"<<outputField.name<<"	"<<this->maxFieldNum<<"	"<<outputField.type<<"****"<<endl;
				}
			}
			this->m_allFileMess.insert(pair<string,Field>(outputField.name,outputField));
			service.getOutputField().push_back(outputField);
			cout<<"----"<<outputField.name<<"	"<<outputField.size<<"	"<<outputField.type<<"----"<<endl;
		}
	}
	cout<<"=====处理crmdem.fml32文件完成====="<<endl;
	//service.printService();
	return n_acmanager::RETURN_SUCCESS;
}

int ACConfManager::parseMetadataIn(ACService& service)
{

	//服务头字符串
	string serviceString=replaceString(serviceTemp,"@SERVICE@",service.getBaseMessage().name);

	//输入字段字符串
	string inputString="";
	string inputNodeString="";
	string inputFieldsString="";
	vector<Field>& inputFields=service.getInputField();
	for(Field field:inputFields)
	{
		if(field.type==FieldType::FML32)
		{
			inputNodeString=replaceString(nodeTemp,"@NODE@",field.name);
			inputNodeString=replaceString(inputNodeString,"@ACCESS@","in");

			inputNodeString=replaceString(inputNodeString,"@SIZE@",longToString(field.size));
		}
		else
		{
			string fieldTemp=fieldTemp1;
			if(service.getBaseMessage().input_node!="NULL")
			{
				fieldTemp=fieldTemp2;
			}
			string inputFieldString=replaceString(fieldTemp,"@PARAM@",field.name);
			inputFieldString=replaceString(inputFieldString,"@ACCESS@","in");
			inputFieldString=replaceString(inputFieldString,"@SIZE@",longToString(field.size));
			if(field.type==FieldType::LONG)
			{
				inputFieldString=replaceString(inputFieldString,"@TYPE@","long");
			}
			else if(field.type==FieldType::STRING)
			{
				inputFieldString=replaceString(inputFieldString,"@TYPE@","string");
			}
			inputFieldsString+=inputFieldString;
		}
	}
	if(inputNodeString!="" && inputFieldsString!="")
	{
		inputString=replaceString(inputNodeString,"@FIELDS@",inputFieldsString);
	}
	else if(inputFieldsString!="")
	{
		inputString=inputFieldsString;
	}

	//输出字段字符串
	string outputString="";
	string outputNodeString="";
	string outputFieldsString="";
	vector<Field>& outputFields=service.getOutputField();
	for(Field field:outputFields)
	{
		if(field.type==FieldType::FML32)
		{
			outputNodeString=replaceString(nodeTemp,"@NODE@",field.name);
			outputNodeString=replaceString(outputNodeString,"@ACCESS@","out");
			outputNodeString=replaceString(outputNodeString,"@SIZE@",longToString(field.size));
		}
		else
		{
			string fieldTemp=fieldTemp1;
			if(service.getBaseMessage().output_node!="NULL")
			{
				fieldTemp=fieldTemp2;
			}
			string outputFieldString=replaceString(fieldTemp,"@PARAM@",field.name);
			outputFieldString=replaceString(outputFieldString,"@ACCESS@","out");
			outputFieldString=replaceString(outputFieldString,"@SIZE@",longToString(field.size));
			if(field.type==FieldType::LONG)
			{
				outputFieldString=replaceString(outputFieldString,"@TYPE@","long");
			}
			else if(field.type==FieldType::STRING)
			{
				outputFieldString=replaceString(outputFieldString,"@TYPE@","string");
			}
			outputFieldsString+=outputFieldString;
		}
	}
	if(outputNodeString!="" && outputFieldsString!="")
	{
		outputString=replaceString(outputNodeString,"@FIELDS@",outputFieldsString);
	}
	else if(outputFieldsString!="")
	{
		outputString=outputFieldsString;
	}

	//写入metadata.in文件
	string metadataString=serviceString+inputString+outputString;
	cout<<metadataString<<endl;
	if(writeMetadataIn(metadataString,service.getBaseMessage().name)!=n_acmanager::RETURN_SUCCESS)
	{
		cout<<"=====处理metadata.in文件失败====="<<endl;
		return n_acmanager::RETURN_FAILED;
	}
	else
	{
		cout<<"=====处理metadata.in文件完成====="<<endl;
		return n_acmanager::RETURN_SUCCESS;
	}
}

int ACConfManager::writeCrmdemFml32(Field& Field)
{
	ofstream ofstre;
	ofstre.open(this->crmdemFileName,ofstream::app);
	if(ofstre.is_open())
	{
		ostringstream ostringstre;
		ostringstre<<Field.name;
		for(int i=Field.name.size();i<35;i++)
		{
			ostringstre<<" ";
		}
		ostringstre<<(++this->maxFieldNum)<<"    ";
		if(Field.type==FieldType::LONG)
			ostringstre<<"long"<<"   ";
		else if(Field.type==FieldType::STRING)
			ostringstre<<"string"<<"   ";
		else if(Field.type==FieldType::FML32)
			ostringstre<<"fml32"<<"    ";

		ostringstre<<"-    -\n";

		string temp=ostringstre.str();
		ofstre<<temp;
		//ofstre<<ostringstre.str();
		ofstre.flush();
		ofstre.close();
	}
	else
	{
		cout<<"=====写入crmdem.fml32文件异常====="<<endl;
		ofstre.close();
		return n_acmanager::RETURN_FAILED;
	}

	return n_acmanager::RETURN_SUCCESS;
}

//写入metadata.in文件
int ACConfManager::writeMetadataIn(string serverTemp,string serviceName)
{
	if(this->s_services.find(serviceName)==this->s_services.end())
	{
		ofstream ofstre;
		ofstre.open(this->metadataFileName,ofstream::app | ofstream::binary);
		if(ofstre.is_open())
		{
			ofstre<<serverTemp<<endl;
			ofstre.flush();
			ofstre.close();
		}
		else
		{
			cout<<"=====写入metadata.in文件异常====="<<endl;
			ofstre.close();
			return n_acmanager::RETURN_FAILED;
		}
		return n_acmanager::RETURN_SUCCESS;
	}
	else
	{
		cout<<"=====服务"<<serviceName<<"配置已存在，无需写入metadata.in文件====="<<endl;
		return n_acmanager::RETURN_SUCCESS;
	}
}
