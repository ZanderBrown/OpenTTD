/*
 * Copyright (c) 2003-2011 Alberto Demichelis
 *
 * This software is provided 'as-is', without any
 * express or implied warranty. In no event will the
 * authors be held liable for any damages arising from
 * the use of this software.
 *
 * Permission is granted to anyone to use this software
 * for any purpose, including commercial applications,
 * and to alter it and redistribute it freely, subject
 * to the following restrictions:
 *
 *   1. The origin of this software must not be
 *   misrepresented; you must not claim that
 *   you wrote the original software. If you
 *   use this software in a product, an
 *   acknowledgment in the product
 *   documentation would be appreciated but is
 *   not required.
 *
 *   2. Altered source versions must be plainly
 *   marked as such, and must not be
 *   misrepresented as being the original
 *   software.
 *
 *   3. This notice may not be removed or
 *   altered from any source distribution.
 */
#ifndef _SQUIRREL_H_
#define _SQUIRREL_H_

#include "../../../string_type.h"

typedef int64_t SQInteger;
typedef uint64_t SQUnsignedInteger;
typedef uint64_t SQHash; /*should be the same size of a pointer*/
typedef int SQInt32;


#ifdef SQUSEDOUBLE
typedef double SQFloat;
#else
typedef float SQFloat;
#endif


struct SQResult {
private:
	enum class Tag {
		OK = 0,
		RETURN = 1, /* a value was left on the stack */
		ERROR = -1,
		SUSPEND = -666,
	} _tag;

	SQResult(Tag tag) : _tag(tag) {}

public:
	static const SQResult OK;
	static const SQResult RETURN;
	static const SQResult ERROR;
	static const SQResult SUSPEND;

	inline constexpr bool Failed() const noexcept {
		return int(this->_tag) < 0;
	}

	inline constexpr bool Succeeded() const noexcept {
		return int(this->_tag) >= 0;
	}

	inline constexpr bool IsOk() const noexcept {
		return this->_tag == Tag::OK;
	}

	inline constexpr bool IsError() const noexcept {
		return this->_tag == Tag::ERROR;
	}

	inline constexpr bool IsReturn() const noexcept {
		return this->_tag == Tag::RETURN;
	}

	inline constexpr bool IsSuspend() const noexcept {
		return this->_tag == Tag::SUSPEND;
	}
};

/** Function completed successfully, no value was left on the stack. */
inline const SQResult SQResult::OK = SQResult(Tag::OK);
/** Function completed successfully, return value is on the stack. */
inline const SQResult SQResult::RETURN = SQResult(Tag::RETURN);
inline const SQResult SQResult::ERROR = SQResult(Tag::ERROR);
/** Used internally */
inline const SQResult SQResult::SUSPEND = SQResult(Tag::SUSPEND);

typedef int64_t SQRawObjectVal; //must be 64bits
#define SQ_OBJECT_RAWINIT() { _unVal.raw = 0; }

typedef void* SQUserPointer;
typedef SQUnsignedInteger SQBool;

#define SQTrue	(1)
#define SQFalse	(0)

struct SQVM;
struct SQTable;
struct SQArray;
struct SQString;
struct SQClosure;
struct SQGenerator;
struct SQNativeClosure;
struct SQUserData;
struct SQFunctionProto;
struct SQRefCounted;
struct SQClass;
struct SQInstance;
struct SQDelegable;

#define MAX_CHAR 0xFFFF

#define SQUIRREL_VERSION	"Squirrel 2.2.5 stable - With custom OpenTTD modifications"
#define SQUIRREL_COPYRIGHT	"Copyright (C) 2003-2010 Alberto Demichelis"
#define SQUIRREL_AUTHOR		"Alberto Demichelis"
#define SQUIRREL_VERSION_NUMBER	225

#define SQ_VMSTATE_IDLE			0
#define SQ_VMSTATE_RUNNING		1
#define SQ_VMSTATE_SUSPENDED	2

#define SQUIRREL_EOB 0
#define SQ_BYTECODE_STREAM_TAG	0xFAFA

#define SQOBJECT_REF_COUNTED	0x08000000
#define SQOBJECT_NUMERIC		0x04000000
#define SQOBJECT_DELEGABLE		0x02000000
#define SQOBJECT_CANBEFALSE		0x01000000

#define SQ_MATCHTYPEMASKSTRING (-99999)

#define _RT_MASK 0x00FFFFFF
#define _RAW_TYPE(type) (type&_RT_MASK)

#define _RT_NULL			0x00000001
#define _RT_INTEGER			0x00000002
#define _RT_FLOAT			0x00000004
#define _RT_BOOL			0x00000008
#define _RT_STRING			0x00000010
#define _RT_TABLE			0x00000020
#define _RT_ARRAY			0x00000040
#define _RT_USERDATA		0x00000080
#define _RT_CLOSURE			0x00000100
#define _RT_NATIVECLOSURE	0x00000200
#define _RT_GENERATOR		0x00000400
#define _RT_USERPOINTER		0x00000800
#define _RT_THREAD			0x00001000
#define _RT_FUNCPROTO		0x00002000
#define _RT_CLASS			0x00004000
#define _RT_INSTANCE		0x00008000
#define _RT_WEAKREF			0x00010000

typedef enum tagSQObjectType{
	OT_NULL =			(_RT_NULL|SQOBJECT_CANBEFALSE),
	OT_INTEGER =		(_RT_INTEGER|SQOBJECT_NUMERIC|SQOBJECT_CANBEFALSE),
	OT_FLOAT =			(_RT_FLOAT|SQOBJECT_NUMERIC|SQOBJECT_CANBEFALSE),
	OT_BOOL =			(_RT_BOOL|SQOBJECT_CANBEFALSE),
	OT_STRING =			(_RT_STRING|SQOBJECT_REF_COUNTED),
	OT_TABLE =			(_RT_TABLE|SQOBJECT_REF_COUNTED|SQOBJECT_DELEGABLE),
	OT_ARRAY =			(_RT_ARRAY|SQOBJECT_REF_COUNTED),
	OT_USERDATA =		(_RT_USERDATA|SQOBJECT_REF_COUNTED|SQOBJECT_DELEGABLE),
	OT_CLOSURE =		(_RT_CLOSURE|SQOBJECT_REF_COUNTED),
	OT_NATIVECLOSURE =	(_RT_NATIVECLOSURE|SQOBJECT_REF_COUNTED),
	OT_GENERATOR =		(_RT_GENERATOR|SQOBJECT_REF_COUNTED),
	OT_USERPOINTER =	_RT_USERPOINTER,
	OT_THREAD =			(_RT_THREAD|SQOBJECT_REF_COUNTED) ,
	OT_FUNCPROTO =		(_RT_FUNCPROTO|SQOBJECT_REF_COUNTED), //internal usage only
	OT_CLASS =			(_RT_CLASS|SQOBJECT_REF_COUNTED),
	OT_INSTANCE =		(_RT_INSTANCE|SQOBJECT_REF_COUNTED|SQOBJECT_DELEGABLE),
	OT_WEAKREF =		(_RT_WEAKREF|SQOBJECT_REF_COUNTED)
}SQObjectType;

#define ISREFCOUNTED(t) (t&SQOBJECT_REF_COUNTED)


typedef union tagSQObjectValue
{
	struct SQTable *pTable;
	struct SQArray *pArray;
	struct SQClosure *pClosure;
	struct SQGenerator *pGenerator;
	struct SQNativeClosure *pNativeClosure;
	struct SQString *pString;
	struct SQUserData *pUserData;
	SQInteger nInteger;
	SQFloat fFloat;
	SQUserPointer pUserPointer;
	struct SQFunctionProto *pFunctionProto;
	struct SQRefCounted *pRefCounted;
	struct SQDelegable *pDelegable;
	struct SQVM *pThread;
	struct SQClass *pClass;
	struct SQInstance *pInstance;
	struct SQWeakRef *pWeakRef;
	SQRawObjectVal raw;
}SQObjectValue;


typedef struct tagSQObject
{
	SQObjectType _type;
	SQObjectValue _unVal;
}SQObject;

typedef struct tagSQStackInfos{
	std::string_view funcname;
	std::string_view source;
	SQInteger line = -1;
}SQStackInfos;

typedef struct SQVM* HSQUIRRELVM;
typedef SQObject HSQOBJECT;
typedef SQResult (*SQFUNCTION) (HSQUIRRELVM);
typedef SQResult (*SQRELEASEHOOK) (SQUserPointer, SQInteger size);
typedef void (*SQCOMPILERERROR)(HSQUIRRELVM,std::string_view /*desc*/,std::string_view /*source*/,SQInteger /*line*/,SQInteger /*column*/);
typedef void (*SQPRINTFUNCTION)(HSQUIRRELVM,std::string_view);

typedef SQInteger (*SQWRITEFUNC)(SQUserPointer,SQUserPointer,SQInteger);
typedef SQInteger (*SQREADFUNC)(SQUserPointer,SQUserPointer,SQInteger);

typedef char32_t (*SQLEXREADFUNC)(SQUserPointer);

typedef struct tagSQRegFunction{
	std::string_view name;
	SQFUNCTION f;
	SQInteger nparamscheck;
	std::optional<std::string_view> typemask;
}SQRegFunction;

typedef struct tagSQFunctionInfo {
	SQUserPointer funcid;
	std::string_view name;
	std::string_view source;
}SQFunctionInfo;

/*vm*/
bool sq_can_suspend(HSQUIRRELVM v);
HSQUIRRELVM sq_open(SQInteger initialstacksize);
HSQUIRRELVM sq_newthread(HSQUIRRELVM friendvm, SQInteger initialstacksize);
void sq_seterrorhandler(HSQUIRRELVM v);
void sq_close(HSQUIRRELVM v);
void sq_setforeignptr(HSQUIRRELVM v,SQUserPointer p);
SQUserPointer sq_getforeignptr(HSQUIRRELVM v);
void sq_setprintfunc(HSQUIRRELVM v, SQPRINTFUNCTION printfunc);
SQPRINTFUNCTION sq_getprintfunc(HSQUIRRELVM v);
SQResult sq_suspendvm(HSQUIRRELVM v);
bool sq_resumecatch(HSQUIRRELVM v, int suspend = -1);
bool sq_resumeerror(HSQUIRRELVM v);
SQResult sq_wakeupvm(HSQUIRRELVM v, SQBool resumedret, SQBool retval, SQBool raiseerror, SQBool throwerror);
SQInteger sq_getvmstate(HSQUIRRELVM v);
void sq_decreaseops(HSQUIRRELVM v, int amount);

/*compiler*/
SQResult sq_compile(HSQUIRRELVM v, SQLEXREADFUNC read, SQUserPointer p, std::string_view sourcename, SQBool raiseerror);
SQResult sq_compilebuffer(HSQUIRRELVM v, std::string_view buffer, std::string_view sourcename, SQBool raiseerror);
void sq_enabledebuginfo(HSQUIRRELVM v, SQBool enable);
void sq_notifyallexceptions(HSQUIRRELVM v, SQBool enable);
void sq_setcompilererrorhandler(HSQUIRRELVM v,SQCOMPILERERROR f);

/*stack operations*/
void sq_push(HSQUIRRELVM v,SQInteger idx);
void sq_pop(HSQUIRRELVM v,SQInteger nelemstopop);
void sq_poptop(HSQUIRRELVM v);
void sq_remove(HSQUIRRELVM v,SQInteger idx);
SQInteger sq_gettop(HSQUIRRELVM v);
void sq_settop(HSQUIRRELVM v,SQInteger newtop);
void sq_reservestack(HSQUIRRELVM v,SQInteger nsize);
SQInteger sq_cmp(HSQUIRRELVM v);
void sq_move(HSQUIRRELVM dest,HSQUIRRELVM src,SQInteger idx);

/*object creation handling*/
SQUserPointer sq_newuserdata(HSQUIRRELVM v,SQUnsignedInteger size);
void sq_newtable(HSQUIRRELVM v);
void sq_newarray(HSQUIRRELVM v,SQInteger size);
void sq_newclosure(HSQUIRRELVM v,SQFUNCTION func,SQUnsignedInteger nfreevars);
SQResult sq_setparamscheck(HSQUIRRELVM v, SQInteger nparamscheck, std::optional<std::string_view> typemask);
SQResult sq_bindenv(HSQUIRRELVM v, SQInteger idx);
void sq_pushstring(HSQUIRRELVM v, std::string_view str);
void sq_pushfloat(HSQUIRRELVM v,SQFloat f);
void sq_pushinteger(HSQUIRRELVM v,SQInteger n);
void sq_pushbool(HSQUIRRELVM v,SQBool b);
void sq_pushuserpointer(HSQUIRRELVM v,SQUserPointer p);
void sq_pushnull(HSQUIRRELVM v);
SQObjectType sq_gettype(HSQUIRRELVM v,SQInteger idx);
SQInteger sq_getsize(HSQUIRRELVM v,SQInteger idx);
SQResult sq_getbase(HSQUIRRELVM v, SQInteger idx);
SQBool sq_instanceof(HSQUIRRELVM v);
void sq_tostring(HSQUIRRELVM v,SQInteger idx);
void sq_tobool(HSQUIRRELVM v, SQInteger idx, SQBool *b);
SQResult sq_getstring(HSQUIRRELVM v, SQInteger idx, std::string_view &str);
SQResult sq_getinteger(HSQUIRRELVM v, SQInteger idx, SQInteger *i);
SQResult sq_getfloat(HSQUIRRELVM v, SQInteger idx, SQFloat *f);
SQResult sq_getbool(HSQUIRRELVM v, SQInteger idx, SQBool *b);
SQResult sq_getthread(HSQUIRRELVM v, SQInteger idx, HSQUIRRELVM *thread);
SQResult sq_getuserpointer(HSQUIRRELVM v, SQInteger idx, SQUserPointer *p);
SQResult sq_getuserdata(HSQUIRRELVM v, SQInteger idx, SQUserPointer *p, SQUserPointer *typetag);
SQResult sq_settypetag(HSQUIRRELVM v, SQInteger idx, SQUserPointer typetag);
SQResult sq_gettypetag(HSQUIRRELVM v, SQInteger idx, SQUserPointer *typetag);
void sq_setreleasehook(HSQUIRRELVM v,SQInteger idx,SQRELEASEHOOK hook);
std::span<char> sq_getscratchpad(HSQUIRRELVM v,SQInteger minsize);
SQResult sq_getfunctioninfo(HSQUIRRELVM v, SQInteger idx, SQFunctionInfo *fi);
SQResult sq_getclosureinfo(HSQUIRRELVM v, SQInteger idx, SQUnsignedInteger *nparams, SQUnsignedInteger *nfreevars);
SQResult sq_setnativeclosurename(HSQUIRRELVM v, SQInteger idx, std::string_view name);
SQResult sq_setinstanceup(HSQUIRRELVM v, SQInteger idx, SQUserPointer p);
SQResult sq_getinstanceup(HSQUIRRELVM v, SQInteger idx, SQUserPointer *p, SQUserPointer typetag);
SQResult sq_setclassudsize(HSQUIRRELVM v, SQInteger idx, SQInteger udsize);
SQResult sq_newclass(HSQUIRRELVM v, SQBool hasbase);
SQResult sq_createinstance(HSQUIRRELVM v, SQInteger idx);
SQResult sq_setattributes(HSQUIRRELVM v, SQInteger idx);
SQResult sq_getattributes(HSQUIRRELVM v, SQInteger idx);
SQResult sq_getclass(HSQUIRRELVM v,SQInteger idx);
void sq_weakref(HSQUIRRELVM v,SQInteger idx);
SQResult sq_getdefaultdelegate(HSQUIRRELVM v, SQObjectType t);

/*object manipulation*/
void sq_pushroottable(HSQUIRRELVM v);
void sq_pushregistrytable(HSQUIRRELVM v);
void sq_pushconsttable(HSQUIRRELVM v);
SQResult sq_setroottable(HSQUIRRELVM v);
SQResult sq_setconsttable(HSQUIRRELVM v);
SQResult sq_newslot(HSQUIRRELVM v, SQInteger idx, SQBool bstatic);
SQResult sq_deleteslot(HSQUIRRELVM v, SQInteger idx, SQBool pushval);
SQResult sq_set(HSQUIRRELVM v, SQInteger idx);
SQResult sq_get(HSQUIRRELVM v, SQInteger idx);
SQResult sq_rawget(HSQUIRRELVM v, SQInteger idx);
SQResult sq_rawset(HSQUIRRELVM v, SQInteger idx);
SQResult sq_rawdeleteslot(HSQUIRRELVM v, SQInteger idx, SQBool pushval);
SQResult sq_arrayappend(HSQUIRRELVM v, SQInteger idx);
SQResult sq_arraypop(HSQUIRRELVM v, SQInteger idx, SQBool pushval);
SQResult sq_arrayresize(HSQUIRRELVM v, SQInteger idx, SQInteger newsize);
SQResult sq_arrayreverse(HSQUIRRELVM v, SQInteger idx);
SQResult sq_arrayremove(HSQUIRRELVM v, SQInteger idx, SQInteger itemidx);
SQResult sq_arrayinsert(HSQUIRRELVM v, SQInteger idx, SQInteger destpos);
SQResult sq_setdelegate(HSQUIRRELVM v, SQInteger idx);
SQResult sq_getdelegate(HSQUIRRELVM v, SQInteger idx);
SQResult sq_clone(HSQUIRRELVM v,SQInteger idx);
SQResult sq_setfreevariable(HSQUIRRELVM v, SQInteger idx, SQUnsignedInteger nval);
SQResult sq_next(HSQUIRRELVM v,SQInteger idx);
SQResult sq_getweakrefval(HSQUIRRELVM v, SQInteger idx);
SQResult sq_clear(HSQUIRRELVM v,SQInteger idx);

/*calls*/
SQResult sq_call(HSQUIRRELVM v, SQInteger params, SQBool retval, SQBool raiseerror, int suspend = -1);
SQResult sq_resume(HSQUIRRELVM v, SQBool retval, SQBool raiseerror);
std::optional<std::string_view> sq_getlocal(HSQUIRRELVM v, SQUnsignedInteger level,SQUnsignedInteger idx);
std::optional<std::string_view> sq_getfreevariable(HSQUIRRELVM v,SQInteger idx,SQUnsignedInteger nval);
SQResult sq_throwerror(HSQUIRRELVM v, std::string_view err);
void sq_reseterror(HSQUIRRELVM v);
void sq_getlasterror(HSQUIRRELVM v);

/*raw object handling*/
SQResult sq_getstackobj(HSQUIRRELVM v, SQInteger idx, HSQOBJECT *po);
void sq_pushobject(HSQUIRRELVM v,HSQOBJECT obj);
void sq_addref(HSQUIRRELVM v,HSQOBJECT *po);
SQBool sq_release(HSQUIRRELVM v,HSQOBJECT *po);
void sq_resetobject(HSQOBJECT *po);
std::optional<std::string_view> sq_objtostring(HSQOBJECT *o);
SQBool sq_objtobool(HSQOBJECT *o);
SQInteger sq_objtointeger(HSQOBJECT *o);
SQFloat sq_objtofloat(HSQOBJECT *o);
SQResult sq_getobjtypetag(HSQOBJECT *o, SQUserPointer *typetag);

/*GC*/
SQInteger sq_collectgarbage(HSQUIRRELVM v);

/*serialization*/
SQResult sq_writeclosure(HSQUIRRELVM vm, SQWRITEFUNC writef, SQUserPointer up);
SQResult sq_readclosure(HSQUIRRELVM vm, SQREADFUNC readf, SQUserPointer up);

/*mem allocation*/
void *sq_malloc(SQUnsignedInteger size);
void *sq_realloc(void* p,SQUnsignedInteger oldsize,SQUnsignedInteger newsize);
void sq_free(void *p,SQUnsignedInteger size);

/*debug*/
SQResult sq_stackinfos(HSQUIRRELVM v, SQInteger level, SQStackInfos *si);
void sq_setdebughook(HSQUIRRELVM v);

/*UTILITY MACRO*/
#define sq_isnumeric(o) ((o)._type&SQOBJECT_NUMERIC)
#define sq_istable(o) ((o)._type==OT_TABLE)
#define sq_isarray(o) ((o)._type==OT_ARRAY)
#define sq_isfunction(o) ((o)._type==OT_FUNCPROTO)
#define sq_isclosure(o) ((o)._type==OT_CLOSURE)
#define sq_isgenerator(o) ((o)._type==OT_GENERATOR)
#define sq_isnativeclosure(o) ((o)._type==OT_NATIVECLOSURE)
#define sq_isstring(o) ((o)._type==OT_STRING)
#define sq_isinteger(o) ((o)._type==OT_INTEGER)
#define sq_isfloat(o) ((o)._type==OT_FLOAT)
#define sq_isuserpointer(o) ((o)._type==OT_USERPOINTER)
#define sq_isuserdata(o) ((o)._type==OT_USERDATA)
#define sq_isthread(o) ((o)._type==OT_THREAD)
#define sq_isnull(o) ((o)._type==OT_NULL)
#define sq_isclass(o) ((o)._type==OT_CLASS)
#define sq_isinstance(o) ((o)._type==OT_INSTANCE)
#define sq_isbool(o) ((o)._type==OT_BOOL)
#define sq_isweakref(o) ((o)._type==OT_WEAKREF)
#define sq_type(o) ((o)._type)

/* Limit the total number of ops that can be consumed by an operation */
struct SQOpsLimiter {
	SQOpsLimiter(HSQUIRRELVM v, SQInteger ops, std::string_view label);
	~SQOpsLimiter();
private:
	HSQUIRRELVM _v;
	SQInteger _ops;
};

/* deprecated */
#define sq_createslot(v,n) sq_newslot(v,n,SQFalse)

#endif /*_SQUIRREL_H_*/
