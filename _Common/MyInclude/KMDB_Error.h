#pragma once


#define KMERROR_SUCCESS				0

#define KMERROR_FILEPATH_TOOSHORT	1	//字符变量长度不够容纳文件ID
#define KMERROR_FILEID_TOOSHORT		2	//字符变量长度不够容纳文件ID
#define KMERROR_FILEID_NOTEXIST		7	//数据库中文件ID不存在
#define KMERROR_SPACE_NOTENOUGH		8	//磁盘空间不足
#define KMERROR_FILE_CANNOTIMPORT	9	//该文件没有入库
#define KMERROR_FILE_SEARCH_NOTEXIST 11 //查询的项目不存在

#define KMERROR_USER_CANCEL			14	//用户取消
#define KMERROR_XMLITEMP_NOTEXIST	17	//该XML元素的属性不存在
#define KMERROR_NO_VALUE			18	//值为空
#define KMERROR_MULTI_VALUE			19	//无法容纳多个值
#define KMERROR_OBJECT_NOTFOUND		20	//目标没有发现,不存在
#define KMERROR_STATUS_ERROR		21	//状态错误
#define KMERROR_CANNOT_OPERATE		22	//无法完成请求,如要展开一个文件的链接，但该文件没有链接文件
#define KMERROR_ARRAY_OVERRIDE		23	//数组越界

#define KMERROR_FILE_NOTEXIST			4	//文件不存在
#define KMERROR_FILE_CANNOTMOVE			5	//无法移动文件（可能文件正被使用）
#define KMERROR_FILE_ALREADYEXIST		10	//该文件名已经存在
#define KMERROR_FILE_CANNOTCOPY			12	//无法复制文件
#define KMERROR_FILE_ALREADYINBASE		6	//文档重复入库（文档已经在库中）
#define KMERROR_FILE_NAMEALREADYINBASE	34	//文档名重复入库（文档名已经在库中）
#define KMERROR_FILE_NAMEALREADYIMPORT	35	//该文档名已经被入过至少一次库
#define KMERROR_FILE_NAMEERROR			31	//文件名不正确
#define KMERROR_FILE_DIRERROR			33	//目录错误
#define KMERROR_FILE_NEWVERSION			36	//新版本
#define KMERROR_FILE_INSYSTEMFOLDER		37	//文件在系统目录内，因此无法入库 
#define KMERROR_FILE_ISSYSTEMFOLDER		38	//文件为系统目录本身，因此无法入库 
#define KMERROR_FILE_ISROOTFOLDER		39	//文件为根目录，因此无法入库 
#define KMERROR_FILE_HASRELATIONSHIP	1100//文件具有一些关联文档，因此不能进行删除等操作
#define KMERROR_FILE_OPERATEFAILURE		1101//文件操作（可能为复制、移动、重命名等）失败
#define KMERROR_FILE_INVALIDNAME		1102//无效的文件名

#define KMERROR_NET_NOTSUPPORT			1201//不支持网络


#define KMERROR_XML_FORMATERROR		3	//输入XML包格式错误
#define KMERROR_XMLITEM_NOTEXIST		16	//该XML元素不存在
#define KMERROR_XMLITEM_ALREADYEXIST	40	//该XML元素已经存在
#define KMERROR_XMLITEM_CHANGED			41	//该XML元素已经存在
#define KMERROR_XMLITEM_NOTCHANGED		42	//该XML元素已经存在



//安全控制
#define KMERROR_USER_NOTLOGIN		100	//用户没有登录
#define KMERROR_USER_NOTEXIST		103	//用户不存在
#define KMERROR_USER_PASSWDNOTMATCH	104	//用户密码不匹配
#define KMERROR_USER_ACCESSDECLINE	105	//权限不够
#define KMERROR_USER_REMOTELOGIN	106	//不是本地用户登录


#define KMERROR_CANNOT_CREATE_DIR	101	//不能建立该目录
#define KMERROR_DIRNAME_TOOLONG		102	//目录名太长

#define KMERROR_GMEM_CREATEFAILURE	120	//建立全局内存失败
#define KMERROR_GMEM_LOCKFAILURE	121	//锁定全局内存失败

#define KMERROR_DB_NOTOPEN				30	//数据库没有打开
#define KMERROR_DB_CANNOTUPDATE			32	//无法更新数据库
#define KMERROR_DB_ALREADYLOGIN			130	//文档库已经登录
#define KMERROR_DB_ALREADYLOGOUT		141	//文档库已经登录
#define KMERROR_DB_OTHERUSERLOGIN		136	//文档库已经登录
#define KMERROR_DB_NOTLOGIN				131	//文档库没有登录
#define KMERROR_DB_DATALOGINFAILURE		132	//数据文档库登录失败
#define KMERROR_DB_CONFIGLOGINFAILURE	133	//配置文档库登录失败
#define KMERROR_DB_WRONGUSER			134	//当前用户没有权限
#define KMERROR_DB_NOTFOUND				135	//文档库没找到
#define KMERROR_DB_ACCESSDECLINE		137	//文档库拒绝访问
#define KMERROR_DB_NAMEALREADYEXIST		138	//文档库名称已经存在
#define KMERROR_DB_PATHALREADYEXIST		139	//文档库路径已经被其他文档库使用
#define KMERROR_DB_FILEIDNOTFOUND		140	//文档库中某一个文档ID找不到
#define KMERROR_DB_VERSIONCONFLICT		142	//版本冲突，指某一项已更新，但储存时还是使用了老版本

#define KMERROR_BUFFER_TOSMALL			150	//储存空间太小

#define KMERROR_PASSID_INVALIDATE		160	//PassID无效

#define KMERROR_REG_NOTFOUND			170	//没有发现注册表项
#define KMERROR_REG_FAILURE				171	//注册表项操作失败



#define KMERROR_PARAM_ERROR				250	//传入参数错误
#define KMERROR_EXCEPTION				500	//无法处理的异常错误
#define KMERROR_UNKNOWN					255	//未知错误


#define KMERROR_ARCHIVE_OVERLIMIT		50	//归档文档超过规定数量
#define KMERROR_ARCHIVE_EXIST			51	//该版本已经有了归档文档

#define KMERROR_NAME_ILLEGAL			61	//该版本已经有了归档文档


#define KMERROR_SYSYTEM_NOTINIT			300	//系统没有初始化

#define KMERROR_LOCAL_ONLY				180//仅限本地调用

//extern PA_ERROR s_ErrorInfo[];