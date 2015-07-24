/*
 * ACManager.h
 *
 *  Created on: 2015年5月5日
 *      Author: HDZhang
 */

#ifndef INC_ACMANAGER_H_
#define INC_ACMANAGER_H_

#include<iostream>
#include<map>
#include<ctime>

using namespace std;

namespace n_acmanager
{
	class ACManager
	{
	private:
		//系统配置文件路径
		string configFile;

		/* 保存基本配置值
		 * 作者 AUTOR
		 * 保存结果路径 RESULT_PATH
		 * 时间 TIME
		 */
		map<string,string> m_baseMessage;

		/* 保存服务基本信息
		 * 服务名 SERVICE_NAME
		 * 服务类型 SERVICE_TYPE
		 * 库名 LIB_NAME
		 * SQL SQL_CONTEXT
		 * GROUP_ID
		 * CLASS_ID
		 * 节点名 INPUT_NODE OUTPUT_NODE
		 * 头文件模板 HEAD_TEMPLATE_TYPE
		 * 源文件模板 SOURC_TEMPLATE_TYPE
		 */
		map<string,map<string,string> > m_keyMap;

		/*代码模板映射
		 *
		 */
		map<string,string> m_codeTemplateMap;

	public:
		ACManager(string configFile="./conf/config.ini");
		virtual ~ACManager();

		/**********启动初始化**********
		 * 读取配置文件，根据配置文件读入关键变量值并载入关键字映射和模板
		 * @字符------模板类型
		 * 一般字符----模板路径
		 * #字符------注释
		 */
		int init();

		/**********打印读取的基本信息**********
		 *
		 */
		void showBaseMessage();

		/**********自动生成服务**********
		 *
		 */
		int creatServices();


		//读取配置信息
		int readConfig();
		//根据路径读取关键字模板配置
		int readKeyTemplate(string path);
		//根据路径读取代码模板配置
		int readCodeTemplate(string path);

		//获取关键值
		int getBaseMessage(const string keyName,string codeTemplate);
		//设置关键值
		int setBaseMessage(const string keyName,string codeTemplate);
		//获取特定的关键字映射
		int getKeyMap(const string keyName,map<string,string>& keyMap);
		//获取特定代码模板映射
		int getCodeTemplateMap(const string keyName,string& codeTemplate);
		//插入关键字映射
		int setKeyMap(const string keyName,map<string,string> keyMap);
		//插入代码模板映射
		int setCodeTemplateMap(const string keyName,string codeTemplate);
	};



	//返回值
	const int RETURN_FAILED=0;
	const int RETURN_SUCCESS=1;

	//替换字符串
	string replaceString(string srcString ,string keyString ,string valueString);
	//long转化为string
	string longToString(long longVaiue);
	//获取当前日期
	string getNowDate();
}



#endif /* INC_ACMANAGER_H_ */
