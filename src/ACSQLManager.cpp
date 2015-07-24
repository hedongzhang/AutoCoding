/*
 * ACSQLManager.cpp
 *
 *  Created on: 2015年5月14日
 *      Author: HDZhang
 */

#include"ACSQLManager.h"
#include"ACManager.h"
#include<fstream>
#include<vector>

using namespace n_acmanager;

ACSQLManager::ACSQLManager(ACService service):service(service)
{
	string path=service.m_KeyValue.find("@RESULT_PATH@")->second;
	if(path[path.size()-1]!='/')
	{
		this->serviceFileName=path+"/sql/tb_pub_service.sql";
		this->sqlparamFileName=path+"/sql/tb_pub_sqlparam.sql";
		this->paramcodedetailFileName=path+"/sql/tb_pub_paramcodedetail.sql";
	}
	else
	{
		this->serviceFileName=path+"sql/tb_pub_service.sql";
		this->sqlparamFileName=path+"sql/tb_pub_sqlparam.sql";
		this->paramcodedetailFileName=path+"sql/tb_pub_paramcodedetail.sql";
	}
	complateSQLScript();
}

/*生成sql脚本
 *
 */
int ACSQLManager::complateSQLScript()
{
	if(creatServiceSQL()!=n_acmanager::RETURN_SUCCESS)
	{
		return n_acmanager::RETURN_FAILED;
	}
	if(creatSQLParamSQL()!=n_acmanager::RETURN_SUCCESS)
	{
		return n_acmanager::RETURN_FAILED;
	}
	if(creatParamCodeDetailSQL()!=n_acmanager::RETURN_SUCCESS)
	{
		return n_acmanager::RETURN_FAILED;
	}
	return n_acmanager::RETURN_SUCCESS;
}

//创建服务配置sql脚本
int ACSQLManager::creatServiceSQL()
{
	string serviceSQL=serviceSQLTemp;

	ofstream ofstre;
	ofstre.open(this->serviceFileName , ofstream::app);

	if(ofstre.is_open())
	{
		serviceSQL=replaceString(serviceSQL,"@SERVICE_NAME@",this->service.getBaseMessage().name);
		serviceSQL=replaceString(serviceSQL,"@SERVICE_ID@",this->service.m_KeyValue.find("@SERVICE_ID@")->second);
		serviceSQL=replaceString(serviceSQL,"@CLASS_NAME@",this->service.getOtherMessage().className);
		serviceSQL=replaceString(serviceSQL,"@LIB_NAME@",this->service.getBaseMessage().libName);

		ofstre<<serviceSQL;
		ofstre.flush();
		ofstre.close();
	}
	else
	{
		cout<<endl<<"=====写入tb_pub_service.sql文件异常====="<<endl;
		ofstre.close();
		return n_acmanager::RETURN_FAILED;
	}
	cout<<endl<<"==================="<<endl;
	cout<<serviceSQL<<endl;
	cout<<"=====生成tb_pub_service.sql文件完成====="<<endl;
	return n_acmanager::RETURN_SUCCESS;
}
//创建sql配置sql脚本
int ACSQLManager::creatSQLParamSQL()
{
	string sqlParamSQL=sqlParamSQLTemp;

	ofstream ofstre;
	ofstre.open(this->sqlparamFileName , ofstream::app);
	if(ofstre.is_open())
	{
		sqlParamSQL=replaceString(sqlParamSQL,"@CLASS_ID@",this->service.getBaseMessage().class_id);
		sqlParamSQL=replaceString(sqlParamSQL,"@GROUP_ID@",this->service.getBaseMessage().group_id);
		sqlParamSQL=replaceString(sqlParamSQL,"@SERVICE_NAME@",this->service.getBaseMessage().name);
		sqlParamSQL=replaceString(sqlParamSQL,"@SQL@",replaceString(this->service.getBaseMessage().sql,"'","''"));
		sqlParamSQL=replaceString(sqlParamSQL,"@SQL_DESC@",this->service.m_KeyValue.find("@SQL_DESC@")->second);

		int bindCount=this->service.getInputField().size();
		int colCount=this->service.getOutputField().size();

		if(this->service.getBaseMessage().input_node!="NULL")
			bindCount--;
		if(this->service.getBaseMessage().output_node!="NULL")
			colCount--;

		if(service.getNumInoutBindParam()==0 && service.getNumOutputBindParam()==0)
		{
			sqlParamSQL=replaceString(sqlParamSQL,"@BIND_COUNT@",longToString(0));
			sqlParamSQL=replaceString(sqlParamSQL,"@COL_COUNT@",longToString(1));

		}
		else
		{
			sqlParamSQL=replaceString(sqlParamSQL,"@BIND_COUNT@",longToString(service.getNumInoutBindParam()));
			sqlParamSQL=replaceString(sqlParamSQL,"@COL_COUNT@",longToString(service.getNumOutputBindParam()));
		}

		ofstre<<sqlParamSQL;
		ofstre.flush();
		ofstre.close();
	}
	else
	{
		cout<<endl<<"=====写入tb_pub_sqlparam.sql文件异常====="<<endl;
		ofstre.close();
		return n_acmanager::RETURN_FAILED;
	}
	cout<<endl<<"==================="<<endl;
	cout<<sqlParamSQL<<endl;
	cout<<"=====生成tb_pub_sqlparam.sql文件完成====="<<endl;
	return n_acmanager::RETURN_SUCCESS;
}
//创建参数配置sql脚本
int ACSQLManager::creatParamCodeDetailSQL()
{
	string paramCodedetailSQL;
	string paramCodedetailSQL1;
	string paramCodedetailSQL2;

	ofstream ofstre;
	ofstre.open(this->paramcodedetailFileName , ofstream::app);
	if(ofstre.is_open())
	{
		string paramCodedetailSQL1Temp=replaceString(paramCodedetailSQLTemp1,"@CLASS_ID@",this->service.getBaseMessage().class_id);
		paramCodedetailSQL1Temp=replaceString(paramCodedetailSQL1Temp,"@GROUP_ID@",this->service.getBaseMessage().group_id);
		paramCodedetailSQL1=paramCodedetailSQL1Temp;

		string paramCodedetailSQL2Temp=replaceString(paramCodedetailSQLTemp2,"@CLASS_ID@",this->service.getBaseMessage().class_id);
		paramCodedetailSQL2Temp=replaceString(paramCodedetailSQL2Temp,"@GROUP_ID@",this->service.getBaseMessage().group_id);

		//当既无入参又无出参的时候默认加一个出参
		if(service.getNumInoutBindParam()==0 && service.getNumOutputBindParam()==0)
		{
			string paramCodedetailTemp=replaceString(paramCodedetailSQL2Temp,"@PARAM_TYPE@",longToString(0));
			paramCodedetailTemp=replaceString(paramCodedetailTemp,"@ORDER_ID@",longToString(1));
			paramCodedetailTemp=replaceString(paramCodedetailTemp,"@VALUE_TYPE@",longToString(6));
			paramCodedetailTemp=replaceString(paramCodedetailTemp,"@VALUE_LENGTH@",longToString(0));
			paramCodedetailTemp=replaceString(paramCodedetailTemp,"@FIELD_NAME@","DEFINE");

			paramCodedetailSQL2+=paramCodedetailTemp;
		}

		for(vector<string>::size_type i=0;i<this->service.getNumInoutBindParam();i++)
		{
			if(this->service.getInputField()[i].type==FieldType::FML32)
			{
				continue;
			}
			else
			{
				string paramCodedetailTemp=replaceString(paramCodedetailSQL2Temp,"@PARAM_TYPE@",longToString(1));
				paramCodedetailTemp=replaceString(paramCodedetailTemp,"@ORDER_ID@",longToString(i+1));
				if(this->service.getInputField()[i].type==FieldType::LONG)
				{
					paramCodedetailTemp=replaceString(paramCodedetailTemp,"@VALUE_TYPE@",longToString(6));
				}
				else if(this->service.getInputField()[i].type==FieldType::STRING)
				{
					paramCodedetailTemp=replaceString(paramCodedetailTemp,"@VALUE_TYPE@",longToString(5));
				}
				paramCodedetailTemp=replaceString(paramCodedetailTemp,"@VALUE_LENGTH@",longToString(this->service.getInputField()[i].size));
				paramCodedetailTemp=replaceString(paramCodedetailTemp,"@FIELD_NAME@",this->service.getInputField()[i].name);

				paramCodedetailSQL2+=paramCodedetailTemp;
			}
		}

		for(vector<string>::size_type i=0;i<this->service.getNumOutputBindParam();i++)
		{
			if(this->service.getOutputField()[i].type==FieldType::FML32)
				continue;
			else
			{
				string paramCodedetailTemp=replaceString(paramCodedetailSQL2Temp,"@PARAM_TYPE@",longToString(0));
				paramCodedetailTemp=replaceString(paramCodedetailTemp,"@ORDER_ID@",longToString(i+1));
				if(this->service.getOutputField()[i].type==FieldType::LONG)
					paramCodedetailTemp=replaceString(paramCodedetailTemp,"@VALUE_TYPE@",longToString(6));
				else if(this->service.getOutputField()[i].type==FieldType::STRING)
					paramCodedetailTemp=replaceString(paramCodedetailTemp,"@VALUE_TYPE@",longToString(5));
				paramCodedetailTemp=replaceString(paramCodedetailTemp,"@VALUE_LENGTH@",longToString(this->service.getOutputField()[i].size));
				paramCodedetailTemp=replaceString(paramCodedetailTemp,"@FIELD_NAME@",this->service.getOutputField()[i].name);

				paramCodedetailSQL2+=paramCodedetailTemp;
			}
		}
		paramCodedetailSQL=paramCodedetailSQL1+paramCodedetailSQL2;
		ofstre<<paramCodedetailSQL;
		ofstre.flush();
		ofstre.close();
	}
	else
	{
		cout<<endl<<"=====写入tb_pub_paramcodedetail.sql文件异常====="<<endl;
		ofstre.close();
		return n_acmanager::RETURN_FAILED;
	}
	cout<<endl<<"==================="<<endl;
	cout<<paramCodedetailSQL<<endl;
	cout<<"=====生成tb_pub_paramcodedetail.sql文件完成====="<<endl;
	return n_acmanager::RETURN_SUCCESS;
}
