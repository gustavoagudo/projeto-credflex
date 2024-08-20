

#include <iostream>
#include <iomanip>
#include "mysql.h"
#include <locale>
#include <string>


using namespace std;



// classe produto
struct produto {
    string id;
    string nome;
    string desc;
    string estoque;
    string uso;
    string tot;
};

//prototipos de funçoes
void regEntrada();
string tratarTexto(string frase);
int obterQuantidadeEstoque(MYSQL* connect, string cod);
int obterQuantidadeUso(MYSQL* connect, string cod);
void regSaida();
void regSaidauso(string cod, int estoque, int num, int uso);
void regSaidadevolucao(string cod, int estoque, int num, int tot);
void mostrarProduto(MYSQL*connect, string cod);
void imprimirProduto();
void buscaProduto();
void imprimirRecursos();



int main()
{
    setlocale(LC_ALL, "PT_BR.UTF-8");
    int op = -1;
    
    do {
        system("cls");
        cout << "\n================================================================\n";
        cout << "SEJA BEM VINDO AO SISTEMA DE GERENCIAMENTO DE HARDWARE CREDFLEX";
        cout << "\n================================================================\n";
        cout << "\nEscolha uma Opção: ";
        cout << "\n1 - Registrar entrada/Alterar quantidade";
        cout << "\n2 - Registrar saída/Alterar uso ou estoque";
        cout << "\n3 - Imprimir recursos";
        cout << "\n4 - Apagar registro";
        cout << "\n5 - SAIR\n";

        cin >> op;
        switch (op) {
        case 1:
            regEntrada();
            break;
        case 2:
            regSaida();
            break;
        case 3:
            imprimirRecursos();
            break;
        case 4:
            buscaProduto();
            break;
        }
        
    } while (op != 5);
}

string tratarTexto(string frase) {
    string resultado;
    bool espacoEncontrado = false;

    for (char c : frase) {
        if (isspace(c)) { //se for um espaço
            if (!espacoEncontrado) { //caso o caracter não seja um espaço, concatena com o resto da string
                resultado += toupper(c);
                espacoEncontrado = true;
            }
        }
        else {
            resultado += toupper(c);
            espacoEncontrado = false;
        }
    }
    return resultado;
}

void regEntrada() {
    MYSQL* connect;
    MYSQL_RES* res;
    MYSQL_ROW row;

    connect = mysql_init(NULL);
    if (!mysql_real_connect(connect, "127.0.0.1", "root", "04032005", "credflex", 0, NULL, 0)) {
        cerr << "Erro ao conectar ao banco de dados: " << mysql_error(connect) << endl;
    }

    produto novoProduto;//instancia objeto tipo produto
    string nome, desc;
    int estoque = 0, uso = 0;

    imprimirProduto();

    cin.ignore(80, '\n');
    cout << "\nDigite o nome do produto, para fazer a entrada, ou de novo produto para registrar: ";
    getline(cin, nome);

    //PADRONIZANDO TEXTO
    novoProduto.nome = tratarTexto(nome);
    
    cout << "\nDigite a quantidade do produto a entrar no estoque: ";
    cin >> estoque;

    cout << "\nDigite a quantidade do produto a entrar em uso: ";
    cin >> uso;
    int tot = estoque + uso;
    //buscando no estoque
    string query = "SELECT i_produto_estoque FROM estoque WHERE s_nome_estoque = '" + novoProduto.nome + "'";
    if (mysql_query(connect, query.c_str())) {
        cerr << "Erro ao executar a query: " << mysql_error(connect) << endl;
    }

    res = mysql_store_result(connect); // obtem resultado da consulta
    if (!res) {
        cerr << "Erro ao obter o resultado da consulta: " << mysql_error(connect) << endl;
    }
    if (res) {
        if (mysql_num_rows(res) != NULL) {
            cout << "\nProduto encontrado no estoque!\n";
            
            row = mysql_fetch_row(res);
            string cod = row[0]; // armazena o valor do id em uma variavel
            int quantEstoque_bd = obterQuantidadeEstoque(connect, cod);
            if (quantEstoque_bd >= 0) {
                estoque += quantEstoque_bd;
            }
            else {
                cout << "Erro ao obter quantidade!";
            }
            novoProduto.estoque = to_string(estoque);
            int quantUso_bd = obterQuantidadeUso(connect, cod);
            if (quantUso_bd >= 0) {
                uso += quantUso_bd;
            }
            else {
                cout << "Erro ao obter quantidade!";
            }
            novoProduto.uso = to_string(uso);
            tot = uso + estoque;
            novoProduto.tot = to_string(tot);
            string query = "UPDATE estoque SET i_quantestoque_estoque = '" + novoProduto.estoque + "', i_quantuso_estoque = '"+ novoProduto.uso +"', i_total_estoque = '"+ novoProduto.tot +"' WHERE i_produto_estoque = '"+ cod +"'";
            if (mysql_query(connect, query.c_str())) {
                cerr << "Erro ao executar a query: " << mysql_error(connect) << endl;
            }
            mysql_free_result(res);
        }
        else {
            cin.ignore(80, '\n');
            cout << "\nDigite uma breve descrição do produto: ";
            getline(cin, desc);
            novoProduto.desc = tratarTexto(desc);

            novoProduto.estoque = to_string(estoque);
            novoProduto.uso = to_string(uso);
            novoProduto.tot = to_string(tot);
            string query = "INSERT INTO estoque (s_nome_estoque, s_dsc_estoque, i_quantestoque_estoque, i_quantuso_estoque, i_total_estoque) \
            VALUES ('" + novoProduto.nome + "', '" + novoProduto.desc + "', '" + novoProduto.estoque + "', '" + novoProduto.uso + "', '" + novoProduto.tot + "')";
            if (mysql_query(connect, query.c_str())) {
                cerr << "Erro ao executar a query: " << mysql_error(connect) << endl;
            }
        }
    }
    else {
        cerr << "\nErro ao obter o resultado da consulta: " << mysql_error(connect);
    }
    
    mysql_close(connect);
    
    system("pause");
}


int obterQuantidadeEstoque(MYSQL* connect, string cod) {
    string query = "SELECT i_quantestoque_estoque FROM estoque WHERE i_produto_estoque = '" + cod + "'";
    if (mysql_query(connect, query.c_str())) {
        cerr << "Erro ao executar a query: " << mysql_error(connect) << endl;
        return -1; // Retornar um valor negativo para indicar erro
    }
    MYSQL_RES* res = mysql_store_result(connect);
    if (!res) {
        cerr << "Erro ao obter o resultado da consulta: " << mysql_error(connect) << endl;
        return -1; // Retornar um valor negativo para indicar erro
    }
    MYSQL_ROW row = mysql_fetch_row(res);
    if (!row) {
        // O produto nÃ£o foi encontrado no estoque
        mysql_free_result(res);
        return 0; // Retornar 0 para indicar que nÃ£o hÃ¡ quantidade
    }
    // Converter a quantidade de string para int
    int quantidade = stoi(row[0]);
    mysql_free_result(res);
    return quantidade; // Retornar a quantidade encontrada no estoque
}

int obterQuantidadeUso(MYSQL* connect, string cod) {
    string query = "SELECT i_quantuso_estoque FROM estoque WHERE i_produto_estoque = '" + cod + "'";
    if (mysql_query(connect, query.c_str())) {
        cerr << "Erro ao executar a query: " << mysql_error(connect) << endl;
        return -1; // Retornar um valor negativo para indicar erro
    }
    MYSQL_RES* res = mysql_store_result(connect);
    if (!res) {
        cerr << "Erro ao obter o resultado da consulta: " << mysql_error(connect) << endl;
        return -1; // Retornar um valor negativo para indicar erro
    }
    MYSQL_ROW row = mysql_fetch_row(res);
    if (!row) {
        // O produto nÃ£o foi encontrado no estoque
        mysql_free_result(res);
        return 0; // Retornar 0 para indicar que nÃ£o hÃ¡ quantidade
    }
    // Converter a quantidade de string para int
    int quantidade = stoi(row[0]);
    mysql_free_result(res);
    return quantidade; // Retornar a quantidade encontrada no estoque
}

int obterQuantTot(MYSQL* connect, string cod) {
    string query = "SELECT i_total_estoque FROM estoque WHERE i_produto_estoque = '" + cod + "'";
    if (mysql_query(connect, query.c_str())) {
        cerr << "Erro ao executar a query: " << mysql_error(connect) << endl;
        return -1; // Retornar um valor negativo para indicar erro
    }
    MYSQL_RES* res = mysql_store_result(connect);
    if (!res) {
        cerr << "Erro ao obter o resultado da consulta: " << mysql_error(connect) << endl;
        return -1; // Retornar um valor negativo para indicar erro
    }
    MYSQL_ROW row = mysql_fetch_row(res);
    if (!row) {
        // O produto nÃ£o foi encontrado no estoque
        mysql_free_result(res);
        return 0; // Retornar 0 para indicar que nÃ£o hÃ¡ quantidade
    }
    // Converter a quantidade de string para int
    int quantidade = stoi(row[0]);
    mysql_free_result(res);
    return quantidade; // Retornar a quantidade encontrada no estoque
}

void regSaida() {
    MYSQL* connect;
    MYSQL_RES* res;
    MYSQL_ROW row;

    string nome;

    connect = mysql_init(NULL);
    if (!mysql_real_connect(connect, "127.0.0.1", "root", "04032005", "credflex", 0, NULL, 0)) {
        cerr << "Erro ao conectar ao banco de dados: " << mysql_error(connect) << endl;
    }
    
    imprimirProduto();

    cin.ignore(80, '\n');
    cout << "Digite o nome do produto que deseja registrar uma saida: ";
    getline(cin, nome);

    nome = tratarTexto(nome); //trata o texto deixando ele padronizado

    string query = "SELECT i_produto_estoque FROM estoque WHERE s_nome_estoque = '" + nome + "'";
    if (mysql_query(connect, query.c_str())) {
        cerr << "Erro ao executar a query: " << mysql_error(connect) << endl;
    }
    res = mysql_store_result(connect);
    if (!res) {
        cerr << "Erro ao executar a consulta: " << mysql_error(connect);
    }
    if (res) {
        if (mysql_num_rows(res)) {
            cout << "\nProduto encontrado no estoque!\n";
            int estoque = 0;
            row = mysql_fetch_row(res);
            string cod = row[0]; // armazena o valor do id em uma variavel

            //obtendo valores 
            int quantTot_bd = obterQuantTot(connect, cod);

            int quantEstoque_bd = obterQuantidadeEstoque(connect, cod);
            if (quantEstoque_bd >= 0) {
                estoque += quantEstoque_bd;
            }
            else {
                cout << "Erro ao obter quantidade!";
            }
            int uso = 0;
            int quantUso_bd = obterQuantidadeUso(connect, cod);
            if (quantUso_bd >= 0) {
                uso += quantUso_bd;
            }
            else {
                cout << "Erro ao obter quantidade!";
            }

            mostrarProduto(connect, cod);
            int num = 0;
            cout << "\nDigite a quantidade que queira retirar: ";
            cin >> num;


            int op = -1;
            cout << "\n\nEscolha caso devolução ou uso: ";
            cout << "\n\n1 - Devolução de estoque";
            cout << "\n2 - Uso/Mandar para o estoque\n";
            do {
                cin >> op;
                switch (op) {
                case 1:
                    if (quantEstoque_bd > 0) {
                        if (quantEstoque_bd >= num) {
                            regSaidadevolucao(cod, estoque, num, quantTot_bd);
                            break;
                        }
                    }
                    else {
                        cout << "Sem Produtos no estoque para retirar!! \n\n";
                        system("pause");
                        break;
                    }
                case 2:
                    if (quantUso_bd > 0) {
                        if (quantUso_bd >= num) {
                            regSaidauso(cod, estoque, num, uso);
                            break;
                        }
                    }
                    else {
                        cout << "Sem Produtos no estoque para retirar!! \n\n";
                        system("pause");
                        break;
                    }
                    break;
                    }
            } while (op != 1 && op != 2);
        } 
    }
    mysql_free_result(res);
    mysql_close(connect);

}


//FUNÇÃO DE REGISTRAR SAIDAS
 void regSaidadevolucao(string cod, int estoque, int num, int tot) {
     estoque = estoque - num;
     tot = tot - num;
     //convertendo o tipo de dado
     string estoquestr = to_string(estoque);
     string totstr = to_string(tot);
     //iniciando conexão com o banco
     MYSQL* connect;
     connect = mysql_init(NULL);

     if (!mysql_real_connect(connect, "127.0.0.1", "root", "04032005", "credflex", 0, NULL, 0)) {
         cerr << "Erro ao conectar ao banco de dados: " << mysql_error(connect) << endl;
     }

     string query = "UPDATE estoque SET i_quantestoque_estoque = '" + estoquestr + "', i_total_estoque = '"+ totstr +"' WHERE i_produto_estoque = '" + cod + "'";
     if (mysql_query(connect, query.c_str())) {
         cerr << "Erro ao executar a query: " << mysql_error(connect) << endl;
     }
     else {
         cout << "Atualização ao banco de dados concluida!\n";
     }
     system("pause");
     mysql_close(connect);
}
 
 void regSaidauso(string cod, int estoque, int num, int uso) {
     
     uso = uso - num;
     estoque = estoque + num;
     string estoquestr = to_string(estoque);
     string usostr = to_string(uso);
     MYSQL* connect;
     connect = mysql_init(NULL);

     if (!mysql_real_connect(connect, "127.0.0.1", "root", "04032005", "credflex", 0, NULL, 0)) {
         cerr << "Erro ao conectar ao banco de dados: " << mysql_error(connect) << endl;
     }

     string query = "UPDATE estoque SET i_quantestoque_estoque = '" + estoquestr + "', i_quantuso_estoque = '" + usostr + "' WHERE i_produto_estoque = '" + cod + "'";
     if (mysql_query(connect, query.c_str())) {
         cerr << "Erro ao executar a query: " << mysql_error(connect) << endl;
     }
     else {
         cout << "Atualização ao banco de dados concluida!\n";
     }
     mysql_close(connect);
 }

 void mostrarProduto(MYSQL* connect, string cod) {
     MYSQL_RES* res;
     MYSQL_ROW row;

     connect = mysql_init(NULL);
     if (!mysql_real_connect(connect, "127.0.0.1", "root", "04032005", "credflex", 0, NULL, 0)) {
         cerr << "Erro ao conectar ao banco de dados: " << mysql_error(connect) << endl;
     }

     string query = "SELECT  i_produto_estoque, s_nome_estoque, s_dsc_estoque, i_quantestoque_estoque, i_quantuso_estoque, i_total_estoque FROM estoque WHERE i_produto_estoque = '" + cod + "'";
     if (mysql_query(connect, query.c_str())) {
         cerr << "Erro ao executar a query: " << mysql_error(connect) << endl;
     }
     res = mysql_store_result(connect);

     if (res == NULL) {
         cerr << "Erro ao realizar a query: " << mysql_error(connect);
     }
     cout << "\n    ================================================================================================\n";
     cout << "    ID                NOME              DESCRIÇÂO         QNT. ESTOQUE      QNT.USO          TOTAL";
     cout << "\n    ================================================================================================\n";
     while ((row = mysql_fetch_row(res))) { //enquanto tiver conteudo em res
         //i  menor que o numero de linhas retornada pela consulta
         for (unsigned int i = 0; i < mysql_num_fields(res); i++) {//unsigned int -- inteiro sem sinal
             cout << "    " << left << setw(14) << row[i];

         }
         cout << endl;
     }
     mysql_free_result(res);
     mysql_close(connect);
 }

 void imprimirProduto() {
     MYSQL* connect; //faz a conexão com o banco de dados
     MYSQL_RES* res; //captura os resultados da consulta
     MYSQL_ROW row; //consegue acessar os campos(colunas) de cada linha da consulta

     connect = mysql_init(NULL);
     if (!mysql_real_connect(connect, "127.0.0.1", "root", "04032005", "credflex", 0, NULL, 0)) {
         cerr << "Erro ao conectar ao banco de dados: " << mysql_error(connect) << endl;
     }
     string query = "SELECT * FROM estoque";
     if (mysql_query(connect, query.c_str())) {
         cerr << "Erro ao executar a query: " << mysql_error(connect) << endl;
     }

     res = mysql_store_result(connect);
     if (res == NULL) {
         cerr << "\nErro ao realizar a query: " << mysql_error(connect);
     }
     cout << "\n    ================================================================================================\n";
     cout << "    ID                NOME              DESCRIÇÂO         QNT. ESTOQUE      QNT.USO          TOTAL";
     cout << "\n    ================================================================================================\n";
     while ((row = mysql_fetch_row(res))) { //enquanto tiver conteudo em res

         //i  menor que o numero de linhas retornada pela consulta
         for (unsigned int i = 0; i < mysql_num_fields(res); i++) {//unsigned int -- inteiro sem sinal
             cout << "    " << left << setw(14) << row[i];

         }
         cout << "\n    ------------------------------------------------------------------------------------------------\n";
     }
     cout << "\n    ================================================================================================\n";
     //system("pause");
     mysql_free_result(res);
     mysql_close(connect);
 }

 void buscaProduto() {
     string nome;
     MYSQL* connect;
     
     connect = mysql_init(NULL);
     if (!mysql_real_connect(connect, "127.0.0.1", "root", "04032005", "credflex", 0, NULL, 0)) {
         cerr << "Erro ao conectar ao banco de dados: " << mysql_error(connect) << endl;
     }
     /*cin.ignore(80, '\n');
     cout << "Digite CORRETAMENTE o nome do produto para buscar: ";
     getline(cin, nome);

     nome = tratarTexto(nome);

     MYSQL* connect;
     MYSQL_RES* res;
     MYSQL_ROW row;

     connect = mysql_init(NULL);
     if (!mysql_real_connect(connect, "127.0.0.1", "root", "04032005", "credflex", 0, NULL, 0)) {
         cerr << "Erro ao conectar ao banco de dados: " << mysql_error(connect) << endl;
     }
     string likequery = "%" + nome + "%";
     string query = "SELECT * FROM estoque WHERE s_nome_estoque LIKE ('"+ likequery +"')";
     if (mysql_query(connect, query.c_str())) {
         cerr << "Erro ao executar a query: " << mysql_error(connect) << endl;
     }

     res = mysql_store_result(connect);
     if (res == NULL) {
         cerr << "\nErro ao realizar a query: " << mysql_error(connect);
     }
     row = mysql_fetch_row(res);
     if (row != NULL) {
         cout << "\n    ================================================================================================\n";
         cout << "    ID                NOME              DESCRIÇÂO         QNT. ESTOQUE      QNT.USO          TOTAL";
         cout << "\n    ================================================================================================\n";
         while ((row = mysql_fetch_row(res))) { //enquanto tiver conteudo em res
             //i  menor que o numero de linhas retornada pela consulta
             for (unsigned int i = 0; i < mysql_num_fields(res); i++) {//unsigned int -- inteiro sem sinal
                 cout << "    " << left << setw(14) << row[i];

             }
             cout << endl;
         }

            
         cout << "\n\n\n";
         int op = -1;
         cout << "\nDeseja apagar algum registro? ";
         cout << "\n1 - SIM";
         cout << "\n0 - NÂO";
         cin >> op;

         if (op == 1) {
             string id;
             cout << "\nDigite o ID do que deseja apagar: ";
             cin >> id;
             string query = "DELETE from estoque WHERE i_produto_estoque = '" + id + "'";
             if (mysql_query(connect, query.c_str())) {
                 cerr << "Erro ao executar a query: " << mysql_error(connect) << endl;
             }
         }
     }
     else {
         cout << "\nProduto não encontrado no estoque!\n\n";
     }
     system("pause");
     mysql_free_result(res);
     mysql_close(connect);*/

     imprimirProduto();

     int op = -1;
     cout << "\nDeseja apagar algum registro? ";
     cout << "\n1 - SIM";
     cout << "\n0 - NÂO\n";
     cin >> op;

     if (op == 1) {
         string id;
         cout << "\nDigite o ID do que deseja apagar: ";
         cin >> id;
         string query = "DELETE from estoque WHERE i_produto_estoque = '" + id + "'";
         if (mysql_query(connect, query.c_str())) {
             cerr << "Erro ao executar a query: " << mysql_error(connect) << endl;
         }
     }
     mysql_close(connect);
     

 }
 void imprimirRecursos() {
     imprimirProduto();
     system("pause");
 }

 