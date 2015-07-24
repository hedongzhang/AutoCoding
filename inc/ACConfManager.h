/*
 * ACConfManager.h
 *
 *  Created on: 2015年5月4日
 *      Author: HDZhang
 */

#ifndef INC_ACCONFMANAGER_H_
#define INC_ACCONFMANAGER_H_

#include<vector>
#include<map>
#include<set>
#include"ACManager.h"
#include"ACService.h"
/*
 * 主要功能：完成配置文件的生成工作
 *
 *
 */

class ACConfManager
{
public:
	//以配置文件路径初始化类
	ACConfManager(ACService& service);
	//读取CrmdemFml32文件
	int readCrmdemFml32();
	//读取MetadataIn文件
	int readMetadataIn();
	/* 处理配置文件
	 *
	 */
	int complateConfig(ACService& service);

	//解析crmdem.fml32文件，并将未写入的字段写入
	int parseCrmdemFml32(ACService& service);
	//写入CrmdemFml32文件
	int writeCrmdemFml32(Field& Field);
	//解析metadata.in文件，并将服务配置写入
	int parseMetadataIn(ACService& service);
	//写入metadata.in文件
	int writeMetadataIn(string serverTemp,string serviceName);

private:
	string crmdemFileName;
	string metadataFileName;
	map<string,Field> m_allFileMess;
	set<string> s_services;
	int maxFieldNum;


	const string serviceTemp="\nservice=@SERVICE@\n"
						"export=y\n"
						"servicetype=atom\n"
						"inbuf=FML32\n"
						"outbuf=FML32\n"
						"tuxservice=@SERVICE@\n";

	const string nodeTemp="\tparam=@NODE@\n"
					 "\ttype=fml32\n"
					 "\taccess=@ACCESS@\n"
					 "\tsize=@SIZE@\n"
					 "\trequiredcount=0\n"
					 "\tcount=0\n"
					 "\t(\n"
					 "@FIELDS@\n"
					 "\t)\n\n";

	const string fieldTemp1="\tparam=@PARAM@\n"
					 "\ttype=@TYPE@\n"
					 "\taccess=@ACCESS@\n"
					 "\tsize=@SIZE@\n"
					 "\trequiredcount=1\n"
					 "\tcount=1\n\n";

	const string fieldTemp2="\t\tparam=@PARAM@\n"
						 "\t\ttype=@TYPE@\n"
						 "\t\taccess=@ACCESS@\n"
						 "\t\tsize=@SIZE@\n"
						 "\t\trequiredcount=1\n"
						 "\t\tcount=1\n\n";

};



#endif /* INC_ACCONFMANAGER_H_ */
