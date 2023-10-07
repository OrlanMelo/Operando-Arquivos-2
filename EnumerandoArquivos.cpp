#include <Windows.h>
#include <fileapi.h>
#include <string>
#include <iostream>
#include <fstream>
#pragma comment(lib,"kernel32.lib")

using namespace std;

class cFuncoes
{
private:
public:

	void EnumerarArquivos(wstring Local)
	{
		wstring Todos = L"\\*";
		wstring Pesquisar = Local + Todos;

		int Quantidade = 0;

		HANDLE Find1;

		WIN32_FIND_DATAW Arquivo;
		Find1 = FindFirstFile(Pesquisar.c_str(), &Arquivo);
		if (Find1 == INVALID_HANDLE_VALUE)
			return;
		else
		{
			do
			{
				if (Arquivo.dwFileAttributes == FILE_ATTRIBUTE_ARCHIVE)
				{
					Quantidade += 1;
				}
			} while (FindNextFile(Find1,&Arquivo));

			FindClose(Find1);

			cout << "Quantidade: " << Quantidade << " de arquivos.";
			cout<<'\n';
		}
	}

}Funcoes;

int main()
{

	cout << "O assistente está enumerando arquivos no diretório...";

	Funcoes.EnumerarArquivos(L"C:\\");

	system("pause");
}