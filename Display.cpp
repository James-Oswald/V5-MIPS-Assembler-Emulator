
#include<cstdint>
#include<initializer_list>
#include<iostream>
#include<stdexcept>
#include<string>
#include<vector>

#include<jni.h>
#include<windows.h>

using namespace std;

struct JNI
{
	JavaVM* jvm;
	JNIEnv* env;
	vector<char*> args;
	JNI();
	JNI(int, initializer_list<char*>);
	~JNI();
	string version();
};

JNI::JNI()
:JNI(JNI_VERSION_10, {(char*)"-Djava.class.path=."})
{
}

JNI::JNI(int version, initializer_list<char*> args_)
:args(args_)
{                
	JavaVMOption* options = new JavaVMOption[args.size()];
	for(int i = 0; i < args.size(); i++)
		options[i].optionString = args[i];
	JavaVMInitArgs vm_args;   
	vm_args.version = version;   
	vm_args.nOptions = args.size();   
	vm_args.options = options;
	vm_args.ignoreUnrecognized = true; 
	if (JNI_CreateJavaVM(&jvm, (void**)&env, &vm_args) != JNI_OK)
		throw runtime_error("Failed to create JVM");
	delete options; 
}

string JNI::version()
{
	jint ver = env->GetVersion();
	return to_string((ver>>16)&0x0f) + "." + to_string(ver&0x0f);	
}

JNI::~JNI()
{
	jvm->DestroyJavaVM();
}

class Display
{
	private:
		JNI java;
		jclass displayClass;
		jobject displayObject;
		jmethodID constructor_;
		jmethodID setRegisters_;
		jmethodID setMemory_;
		jmethodID setDisplay_;
	public:
		Display();
		void setRegisters(vector<uint32_t>&, uint32_t);
		void setMemory(vector<uint8_t>&);
		void setDisplay(vector<uint32_t>&);
};

Display::Display()
:java()
{
	displayClass = java.env->FindClass("Display");
	if(displayClass == nullptr) cout<<"Failed to find class\n";//throw runtime_error("Failed to find class");
	constructor_ = java.env->GetMethodID(displayClass, "<init>", "()V");
	if(constructor_ == nullptr) cout<<"Failed to find <init>\n";//throw runtime_error("Failed to find <init>");
	displayObject = java.env->NewObject(displayClass, constructor_);
	if(displayObject == nullptr) cout<<"Failed to create object\n"; //throw runtime_error("Failed to create object");
	setRegisters_ = java.env->GetMethodID(displayClass, "setRegisters", "([I)V");
	if(setRegisters_  == nullptr) cout<<"Failed to find setRegisters\n";//throw runtime_error("Failed to find setRegisters");
	setMemory_ = java.env->GetMethodID(displayClass, "setMemory", "([B)V");
	if(setMemory_  == nullptr) cout<<"Failed to find setMemory\n";//throw runtime_error("Failed to find setMemory");
	setDisplay_ = java.env->GetMethodID(displayClass, "setDisplay", "([I)V");
	if(setMemory_  == nullptr) cout<<"Failed to find setDisplay\n";
}

void Display::setRegisters(vector<uint32_t>& a, uint32_t pc)
{
	jint* buffer = new jint[a.size() + 1];
	for(int i = 0; i < a.size(); i++)
		buffer[i] = (jint)a[i];
	buffer[a.size()] = (jint)pc;
	jintArray par = java.env->NewIntArray(a.size()+1);
	java.env->SetIntArrayRegion(par, 0, a.size()+1, buffer);
	java.env->CallVoidMethod(displayObject, setRegisters_, par);
	java.env->DeleteLocalRef(par);
	delete buffer;
}

void Display::setMemory(vector<uint8_t>& a)
{
	jbyte* buffer = new jbyte[a.size()];
	for(int i = 0; i < a.size(); i++)
		buffer[i] = (jbyte)a[i];
	jbyteArray par = java.env->NewByteArray(a.size());
	java.env->SetByteArrayRegion(par, 0, a.size(), buffer);
	java.env->CallVoidMethod(displayObject, setMemory_, par);
	java.env->DeleteLocalRef(par);
	delete buffer;
}

void Display::setDisplay(vector<uint32_t>& a)
{
	jint* buffer = new jint[a.size()];
	for(int i = 0; i < a.size(); i++)
		buffer[i] = (jint)a[i];
	jintArray par = java.env->NewIntArray(a.size());
	java.env->SetIntArrayRegion(par, 0, a.size(), buffer);
	java.env->CallVoidMethod(displayObject, setDisplay_, par);
	java.env->DeleteLocalRef(par);
	delete buffer;
}
