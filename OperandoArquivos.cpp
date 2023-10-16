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
		{
			cout << "Não foi possível criar o arquivo, verifique o código de erro: " << GetLastError();
		}
		else
		{
			//Esta é uma função assíncrona.
			//Permissão declarada em CreateFile usando o sinalizador FILE_FLAG_OVERLAPPED, para sinalizar uma função assíncrona.
			//OVERLAPPED é inicializado primeiro antes de chamar a função assíncrona.
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
					cout << "Não foi possível obter status de tarefa assíncrona." << GetLastError();
				}
			}
			else
			{
				cout << "Não foi possível escrever em arquivo." << GetLastError();
			}

			//O mesmo arquivo será usado para as outras tarefas, portanto, só feche o identificador quando não for mais necessário.
			//CloseHandle(Arquivo);
		}
	}

	void LerArquivo(wstring Local)
	{
		char DadosLidos[MAX_PATH];

		//Esta é uma função assíncrona.
		//Permissão declarada em CreateFile usando o sinalizador FILE_FLAG_OVERLAPPED, para sinalizar uma função assíncrona.
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
				cout << "Ocorreu uma falha durante a operação de leitura." << GetLastError();
			}
		}
	}

	void CopiarArquivo(wstring Local, wstring NovoLocal)
	{
		BOOL Result;

		//Esta é uma função de cópia simples, sem o método assíncrono e com outras opções avançadas para o gerenciamento.
		//Use CopyFileEx ou CopyFile2 para operações mais complexas e com opções de escolha do usuário.
		Result = CopyFile(Local.c_str(), NovoLocal.c_str(), false);
		if (Result == TRUE)
		{
			cout << "A operação de cópia foi concluída com sucesso.";
		}
		else
		{
			cout << "Ocorreu um erro durante a operação de cópia." << GetLastError();
		}
	}

	void MoverArquivo(wstring Local, wstring NovoLocal)
	{
		BOOL Result;

		//O sinalizador MOVEFILE_REPLACE_EXISTING irá substituir o arquivo se tiver com o mesmo nome e extensão.
		//A função irá retornar status apenas se o arquivo for completamente movido, conforme sinalizado em MOVEFILE_WRITE_THROUGH.
		Result = MoveFileEx(Local.c_str(), NovoLocal.c_str(), MOVEFILE_REPLACE_EXISTING| MOVEFILE_WRITE_THROUGH);
		if (Result == TRUE)
		{
			cout << "O arquivo foi movido com sucesso para o novo local.";
		}
		else
		{
			cout << "Ocorreu um erro durante a operação." << GetLastError();
		}
	}

	void ObterTamanhoDoArquivo()
	{
		LARGE_INTEGER Tamanho;

		//Usamos o identificador obtido em CreateFile para esta operação.
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
	cout << "O assistente está efetuando operações em um arquivo...";

	wstring LocalPadrao = L"C:\\Teste.txt";

	//Poderá usar qualquer função.
	Funcoes.CriarArquivo(LocalPadrao, "Texto1");
	Funcoes.LerArquivo(LocalPadrao);
	Funcoes.ObterTamanhoDoArquivo();
	Funcoes.CopiarArquivo(LocalPadrao, L"C:\\TESTE\\Teste.txt");
	Funcoes.MoverArquivo(L"C:\\TESTE\\Teste.txt", LocalPadrao);
	Funcoes.DeletarArquivo(LocalPadrao);//Deletar o arquivo após todas as operações terem sido concluídas.

	system("pause");
}
