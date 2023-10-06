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

	HANDLE Arquivo;

	OVERLAPPED Assincrona;

public:

	void CriarArquivo(wstring Local, string Dados)
	{
		Arquivo = CreateFile(Local.c_str(), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
			0, CREATE_ALWAYS, FILE_FLAG_OVERLAPPED, 0);

		if (Arquivo == INVALID_HANDLE_VALUE)
			cout << "N�o foi poss�vel criar o arquivo, verifique o c�digo de erro: " << GetLastError();
		else
		{
			//Esta � uma fun��o ass�ncrona.
			//Permiss�o declarada em CreateFile usando o sinalizador FILE_FLAG_OVERLAPPED, para sinalizar uma fun��o ass�ncrona.
			//OVERLAPPED � inicializado primeiro antes de chamar a fun��o ass�ncrona.
			Assincrona.hEvent = CreateEvent(0, true, false, 0);

			BOOL Result;
			Result = WriteFile(Arquivo, Dados.c_str(), strlen(Dados.c_str()), 0, &Assincrona);
			if (GetLastError() == ERROR_IO_PENDING)
			{
				DWORD Reservado;
				Result = GetOverlappedResult(Arquivo, &Assincrona, &Reservado, true);
				if (Result == TRUE)
				{
					cout << "Arquivo criado e inserido o texto com sucesso.";
				}
				else
				{
					cout << "N�o foi poss�vel obter status de tarefa ass�ncrona." << GetLastError();
				}
			}
			else
			{
				cout << "N�o foi poss�vel escrever em arquivo." << GetLastError();
			}

			//O mesmo arquivo ser� usado para as outras tarefas, portanto, s� feche o identificador quando n�o for mais necess�rio.
			//CloseHandle(Arquivo);
		}
	}

	void LerArquivo(wstring Local)
	{
		char DadosLidos[MAX_PATH];

		//Esta � uma fun��o ass�ncrona.
		//Permiss�o declarada em CreateFile usando o sinalizador FILE_FLAG_OVERLAPPED, para sinalizar uma fun��o ass�ncrona.
		BOOL Result;
		Result = ReadFile(Arquivo, DadosLidos, MAX_PATH, 0, &Assincrona);
		if (GetLastError() == ERROR_IO_PENDING)
		{
			DWORD Reservado;
			Result = GetOverlappedResult(Arquivo, &Assincrona, &Reservado, true);
			if (Result == TRUE)
			{
				cout << "Dados lidos com sucesso: " << DadosLidos;
			}
			else
			{
				cout << "Ocorreu uma falha durante a opera��o de leitura." << GetLastError();
			}
		}
	}

	void CopiarArquivo(wstring Local, wstring NovoLocal)
	{
		BOOL Result;

		//Esta � uma fun��o de c�pia simples, sem o m�todo ass�ncrono e com outras op��es avan�adas para o gerenciamento.
		//Use CopyFileEx ou CopyFile2 para opera��es mais complexas e com op��es de escolha do usu�rio.
		Result = CopyFile(Local.c_str(), NovoLocal.c_str(), false);
		if (Result == TRUE)
		{
			cout << "A opera��o de c�pia foi conclu�da com sucesso.";
		}
		else
		{
			cout << "Ocorreu um erro durante a opera��o de c�pia." << GetLastError();
		}
	}

	void MoverArquivo(wstring Local, wstring NovoLocal)
	{
		BOOL Result;

		//O sinalizador MOVEFILE_REPLACE_EXISTING ir� substituir o arquivo se tiver com o mesmo nome e extens�o.
		//A fun��o ir� retornar status apenas se o arquivo for completamente movido, conforme sinalizado em MOVEFILE_WRITE_THROUGH.
		Result = MoveFileEx(Local.c_str(), NovoLocal.c_str(), MOVEFILE_REPLACE_EXISTING| MOVEFILE_WRITE_THROUGH);
		if (Result == TRUE)
		{
			cout << "O arquivo foi movido com sucesso para o novo local.";
		}
		else
		{
			cout << "Ocorreu um erro durante a opera��o." << GetLastError();
		}
	}

	void ObterTamanhoDoArquivo()
	{
		LARGE_INTEGER Tamanho;

		//Usamos o identificador obtido em CreateFile para esta opera��o.
		GetFileSizeEx(Arquivo, &Tamanho);

		long CalcularMB = (double)Tamanho.QuadPart / (1024 * 1024);
		long CalcularGB = (double)Tamanho.QuadPart / (1024 * 1024 * 1024);

		if (CalcularGB > 0 == true)
		{
			cout << "Tamanho do arquivo em GB: " << CalcularGB << " GB";
		}
		else
		{
			if (CalcularMB > 0 == true)
			{
				cout << "Tamanho do arquivo em MB: " << CalcularMB << " MB";
			}
			else
			{
				cout << "Tamanho do arquivo em Bytes: " << Tamanho.QuadPart << " Bytes";
			}
		}

		//Terminamos de usar o arquivo que obtivemos em CreateFile.
		CloseHandle(Arquivo);
	}

	void DeletarArquivo(wstring Arquivo)
	{
		DeleteFile(Arquivo.c_str());
	}

}Funcoes;

int main()
{
	cout << "O assistente est� efetuando opera��es em um arquivo...";

	wstring LocalPadrao = L"C:\\Teste.txt";

	//Poder� usar qualquer fun��o.
	Funcoes.CriarArquivo(LocalPadrao, "Texto1");
	Funcoes.LerArquivo(LocalPadrao);
	Funcoes.ObterTamanhoDoArquivo();
	Funcoes.CopiarArquivo(LocalPadrao, L"C:\\TESTE\\Teste.txt");
	Funcoes.MoverArquivo(L"C:\\TESTE\\Teste.txt", LocalPadrao);
	Funcoes.DeletarArquivo(LocalPadrao);//Deletar o arquivo ap�s todas as opera��es terem sido conclu�das.

	system("pause");
}