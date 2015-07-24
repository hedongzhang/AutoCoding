/*
 * ACSQLManager.h
 *
 *  Created on: 2015年5月14日
 *      Author: HDZhang
 */

#ifndef INC_ACSQLMANAGER_H_
#define INC_ACSQLMANAGER_H_

#include"ACService.h"

class ACSQLManager
{
public:
	ACSQLManager(ACService service);

	/*生成sql脚本
	 *
	 */
	int complateSQLScript();

	//创建服务配置sql脚本
	int creatServiceSQL();
	//创建sql配置sql脚本
	int creatSQLParamSQL();
	//创建参数配置sql脚本
	int creatParamCodeDetailSQL();
private:
	ACService service;

	string serviceFileName;
	string sqlparamFileName;
	string paramcodedetailFileName;

	const string serviceSQLTemp="delete from tb_pub_service t where t.OUTER_SVC_NAME in ('@SERVICE_NAME@') ;\n\n"\
							    "insert into tb_pub_service (SVC_ID, INNER_SVC_NAME, OUTER_SVC_NAME, INVERSE_SVC_CODE, OWNER_DLL_NAME, SVC_DESC, TSC_FLAG, RULE_FLAG)\n"
							    "values (@SERVICE_ID@, '@CLASS_NAME@', '@SERVICE_NAME@', null, '@LIB_NAME@', null, '0', '0');\n\n\n";


	const string sqlParamSQLTemp="delete from tb_pub_sqlparam t where t.CLASS_ID=@CLASS_ID@ and t.GROUP_ID in ('@GROUP_ID@') ;\n\n"
							     "insert into tb_pub_sqlparam (CLASS_ID, CLASS_NAME, SQL_TEXT, DB_TYPE, DB_LINK, SERVER_NAME, PROCESS_ID, COL_COUNT, BIND_COUNT, GROUP_ID, SQL_DESC)\n"
							     "values (@CLASS_ID@, '@SERVICE_NAME@', '@SQL@', '1', null, '@SERVICE_NAME@', 1, @COL_COUNT@, @BIND_COUNT@, '@GROUP_ID@', '@SQL_DESC@');\n\n\n";


	const string paramCodedetailSQLTemp1="delete from tb_pub_paramcodedetail t where t.CLASS_ID=@CLASS_ID@ and t.GROUP_ID in ('@GROUP_ID@') ;\n\n";

	const string paramCodedetailSQLTemp2="insert into tb_pub_paramcodedetail (CLASS_ID, PARAM_TYPE, ORDER_ID, VALUE_TYPE, VALUE_LENGTH, FIELD_NAME, VALUE, GROUP_ID)\n"
									  "values (@CLASS_ID@, '@PARAM_TYPE@', '@ORDER_ID@', '@VALUE_TYPE@', @VALUE_LENGTH@, '@FIELD_NAME@', null, '@GROUP_ID@');\n\n";
};



#endif /* INC_ACSQLMANAGER_H_ */
