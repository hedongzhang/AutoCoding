/*
 * ACService.h
 *
 *  Created on: 2015年5月5日
 *      Author: HDZhang
 */

#ifndef ACSERVICE_H_
#define ACSERVICE_H_

#include<iostream>
#include<vector>
#include<map>


using namespace std;

//字段类型
enum FieldType
{
	STRING,
	LONG,
	FML32,
};
//字段
struct Field
{
public:
	string name;
	FieldType type;
	long size;
	bool isNihil;
	bool needAssembly;

	Field()
	{
		name="";
		type=FieldType::STRING;
		size=64;
		isNihil=false;
		needAssembly=false;
	}
};

//服务类型
const string ACServiceTypeSelect="ATOM_SERVICE_TYPE_SELECT";
const string ACServiceTypeUpdate="ATOM_SERVICE_TYPE_UPDATE";
const string ACServiceTypeInsert="ATOM_SERVICE_TYPE_INSERT";
const string ACServiceTypeDelete="ATOM_SERVICE_TYPE_DELETE";
const string ACServiceTypeCombo="ATOM_SERVICE_TYPE_COMBO";


//服务
class ACService
{

public:
	//保存服务基本信息
	struct BaseMess
	{
		string name;          //服务名
		string class_id;	  //class_id
		string group_id;      //组ID
		string input_node;    //入参节点
		string output_node;   //出参节点
		string libName;       //库名
		string sql;           //sql
	};

	//保存服务其他信息
	struct OtherMess
	{
		string type;          //服务类型
		string className;    //类型名
	};

	/***********构造函数**********
	 *
	 */
	ACService(map<string,string>& baseMessage);
	virtual ~ACService();

	/**********初始化服务**********
	 *
	 */
	int init();

	//根据基本信息初始化相关信息
	int initOtherMessage();
	//生成类名
	int creatClassName(string& className);
	//生成服务类型
	int creatServiceType();

	/**********根据sql初始化输入字段**********
	 * 读取每个[:]之前直到空格的作为输入字段名称
	 * exp:
	 * 		latn_id = :LATN_ID
	   	   	and a.acc_nbr = :ACC_NBR
	        其他都会忽略
	   exp:
	   	   	to_char(a.record_time, 'yyyy-mm-dd hh:mi:ss') data ,
	 */
	int initInputFields();
	/**********根据sql初始化输出字段**********
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
	int initOutputFields();

	/**********从文件读取输入输出字段**********
	 *
	 */
	int readFieldsFile(string path);



	//打印服务信息
	int printService();

	//访问基本信息
	BaseMess& getBaseMessage();
	OtherMess& getOtherMessage();
	//访问输入输出字段
	int setInputField(vector<Field>& field);
	vector<Field>& getInputField();
	int setOutputField(vector<Field>& field);
	vector<Field>& getOutputField();

	//获取SQL绑定参数个数
	int getNumInoutBindParam();
	//获取SQL返回参数个数
	int getNumOutputBindParam();
	map<string,string> m_KeyValue;    //简单关键字值替换
private:
	BaseMess baseMess;    //基本信息
	OtherMess otherMess;  //其他信息

	vector<Field> inputFields;   //输入字段
	vector<Field> outputFields;  //输出字段

	int numInputBindParam;  //SQL绑定参数个数
	int numOutputBindParam;  //SQL返回参数个数
};



#endif /* ACSERVICE_H_ */
