/*
  ALGORITMOS DE PROGRAMACAO - Prof. Me. Joao Victor Ramos
  TRABALHO - MERCADO DIGITAL

  INTEGRANTES DA EQUIPE:
  - NOME COMPLETO DO ALUNO 1
  - NOME COMPLETO DO ALUNO 2
  - NOME COMPLETO DO ALUNO 3
  - NOME COMPLETO DO ALUNO 4

  RESUMO:
  Sistema de vendas simples. Os dados ficam so na memoria (vetores de
  structs) e se perdem quando o programa fecha.
  Limites: 3 clientes, 10 produtos e 3 tipos de produto por carrinho.
*/

#include <stdio.h>
#include <string.h>

/* ================= STRUCTS =================
   Cada struct agrupa dados relacionados em um unico registro. */

struct telefones {
    char fixo[15];
    char movel[16];
};

struct clientes {
    int codigo;
    char nome[50];
    double cpf;
    char sexo;
    struct telefones tel;
};

struct modelos {
    char marca[20];
    char modelo[20];
};

struct produtos {
    int codigo;
    char nome[20];
    double valor;
    struct modelos modelo;
};

/* produto[i] e quantidade[i] formam um par: a quantidade do
   produto da posicao 0 fica na posicao 0 do vetor quantidade. */
struct carrinhos {
    struct clientes cliente;
    struct produtos produto[3];
    int quantidade[3];
    double subtotal;
    double desconto;
    double total;
};

/* ================= PROTOTIPOS ================= */
void mostraMenuInicial();

struct clientes cadastrarCliente();

int geraCodigoCliente(double cpf);

void listarClientes(struct clientes cliente[3]);

struct produtos cadastrarProduto();

int maiuscula(char c);

int geraCodigoProduto(char marca[], char modelo[]);

void listarProdutos(struct produtos produto[10]);

void efetuarVenda(struct clientes cliente[3], struct produtos produto[10], struct carrinhos carrinho);

struct clientes localizaCliente(int codigo, struct clientes cliente[3]);

struct produtos localizaProduto(int codigo, struct produtos produto[10]);

void finalizarVenda(struct carrinhos carrinho);

void listarCarrinho(struct carrinhos carrinho);

double calculaSubtotal(struct carrinhos carrinho);

double calculaTotal(double subtotal, double desconto);

/* ================= MENU ================= */

void mostraMenuInicial() {
    printf("\n+------------------------------------------+\n");
    printf("|             MERCADO DIGITAL              |\n");
    printf("+------------------------------------------+\n");
    printf("| 1 - Cadastrar cliente                    |\n");
    printf("| 2 - Cadastrar produto                    |\n");
    printf("| 3 - Listar clientes                      |\n");
    printf("| 4 - Listar produtos                      |\n");
    printf("| 5 - Efetuar venda                        |\n");
    printf("| 0 - Sair                                 |\n");
    printf("+------------------------------------------+\n");
    printf("Escolha: ");
}

/* ================= PRODUTOS ================= */

/* Letra minuscula -> maiuscula (na ASCII, 'a' esta 32 posicoes
   depois de 'A'). Assim "dell" e "DELL" geram o mesmo codigo. */
int maiuscula(char c) {
    if (c >= 'a' && c <= 'z') {
        c = c - 32;
    }
    return c;
}

/* Codigo do produto pela formula do enunciado:
   C1*31^3 + C2*31^2 + C3*31 + C4
   C1 e C2 = 2 primeiras letras da marca
   C3 e C4 = 2 primeiras letras do modelo
   Exemplo: DELL / PRECISION -> D=68, E=69, P=80, R=82 -> 2094659 */
int geraCodigoProduto(char marca[], char modelo[]) {
    int C1 = maiuscula(marca[0]);
    int C2 = maiuscula(marca[1]);
    int C3 = maiuscula(modelo[0]);
    int C4 = maiuscula(modelo[1]);

    return C1 * 31 * 31 * 31 + C2 * 31 * 31 + C3 * 31 + C4;
}

/* Le os dados do produto e devolve o registro pronto.
   Como o registro nasce zerado ({0}), marca e modelo comecam com
   tamanho 0 e o valor com 0, entao cada while abaixo roda ao menos
   uma vez e so termina quando o dado e aceito. */
struct produtos cadastrarProduto() {
    struct produtos novo = {0};
    printf("\n>>> CADASTRO DE PRODUTO <<<\n");

    printf("Nome do produto: ");
    scanf(" %19[^\n]", novo.nome);

    while (strlen(novo.modelo.marca) < 2) {
        printf("Marca (minimo 2 letras): ");
        scanf(" %19[^\n]", novo.modelo.marca);
        if (strlen(novo.modelo.marca) < 2) {
            printf("Marca muito curta.\n");
        }
    }

    while (strlen(novo.modelo.modelo) < 2) {
        printf("Modelo (minimo 2 letras): ");
        scanf(" %19[^\n]", novo.modelo.modelo);
        if (strlen(novo.modelo.modelo) < 2) {
            printf("Modelo muito curto.\n");
        }
    }

    /* Se digitarem letras no valor, o scanf nao devolve 1: o texto e
       descartado com %*s e o valor fica 0, que e recusado. */
    while (novo.valor <= 0) {
        printf("Valor do produto: R$ ");
        if (scanf("%lf", &novo.valor) != 1) {
            scanf("%*s");
            novo.valor = 0;
        }
        if (novo.valor <= 0) {
            printf("O valor deve ser maior que zero.\n");
        }
    }

    novo.codigo = geraCodigoProduto(novo.modelo.marca, novo.modelo.modelo);
    return novo;
}

/* Vaga vazia do vetor tem codigo 0, entao so mostra as preenchidas */
void listarProdutos(struct produtos produto[10]) {
    printf("\n----- PRODUTOS CADASTRADOS -----\n");
    for (int i = 0; i < 10; i++) {
        if (produto[i].codigo != 0) {
            printf("\n[%d] %s\n", produto[i].codigo, produto[i].nome);
            printf("    Marca: %s | Modelo: %s\n",
                   produto[i].modelo.marca, produto[i].modelo.modelo);
            printf("    Valor: R$ %.2f\n", produto[i].valor);
        }
    }
}

/* Devolve o produto com o codigo informado ou, se nao achar,
   um registro zerado (codigo 0) */
struct produtos localizaProduto(int codigo, struct produtos produto[10]) {
    struct produtos achado = {0};
    for (int i = 0; i < 10; i++) {
        if (produto[i].codigo == codigo) {
            achado = produto[i];
        }
    }
    return achado;
}

/* ================= CLIENTES ================= */

/* Codigo = 6 primeiros digitos do CPF. Dividir 12345678901 por
   100000 da 123456,78901; ao converter para int sobra 123456. */
int geraCodigoCliente(double cpf) {
    return (int)(cpf / 100000);
}

/* Le os dados do cliente e devolve o registro pronto.
   CPF e telefones sao lidos como texto para conferir o tamanho e se
   todos os caracteres sao numeros (entre '0' e '9' na ASCII). */
struct clientes cadastrarCliente() {
    struct clientes novo = {0};
    char cpfTexto[20];
    char digitos[16];
    int ok = 0;   /* 1 = dado aceito, 0 = dado recusado */
    printf("\n>>> CADASTRO DE CLIENTE <<<\n");

    printf("Nome completo: ");
    scanf(" %49[^\n]", novo.nome);

    while (ok == 0) {
        ok = 1;
        printf("CPF (11 numeros, sem pontos ou tracos): ");
        scanf("%19s", cpfTexto);
        if (strlen(cpfTexto) != 11) {
            ok = 0;
        }
        for (int i = 0; cpfTexto[i] != '\0'; i++) {
            if (!(cpfTexto[i] >= '0' && cpfTexto[i] <= '9')) {
                ok = 0;
            }
        }
        if (ok == 0) {
            printf("CPF invalido, digite novamente.\n");
        }
    }
    sscanf(cpfTexto, "%lf", &novo.cpf);

    /* O sexo comeca como 0 (registro zerado), entao o while roda */
    while (novo.sexo != 'M' && novo.sexo != 'F' && novo.sexo != 'm' && novo.sexo != 'f') {
        printf("Sexo (M ou F): ");
        scanf(" %c", &novo.sexo);
        if (novo.sexo != 'M' && novo.sexo != 'F' && novo.sexo != 'm' && novo.sexo != 'f') {
            printf("Sexo invalido.\n");
        }
    }

    /* Telefone fixo: 10 numeros (DDD + 8). O sprintf monta a mascara
       (43) 3210-1000 usando pedacos do texto digitado:
       %.2s = 2 primeiros | digitos + 2 = a partir do 3o | digitos + 6 = a partir do 7o */
    ok = 0;
    while (ok == 0) {
        ok = 1;
        printf("Telefone fixo (DDD + 8 numeros): ");
        scanf("%15s", digitos);
        if (strlen(digitos) != 10) {
            ok = 0;
        }
        for (int i = 0; digitos[i] != '\0'; i++) {
            if (!(digitos[i] >= '0' && digitos[i] <= '9')) {
                ok = 0;
            }
        }
        if (ok == 0) {
            printf("Telefone fixo invalido.\n");
        }
    }
    sprintf(novo.tel.fixo, "(%.2s) %.4s-%.4s",
            digitos, digitos + 2, digitos + 6);

    /* Telefone movel: 11 numeros (DDD + 9), mascara (43) 99999-8888 */
    ok = 0;
    while (ok == 0) {
        ok = 1;
        printf("Telefone movel (DDD + 9 numeros): ");
        scanf("%15s", digitos);
        if (strlen(digitos) != 11) {
            ok = 0;
        }
        for (int i = 0; digitos[i] != '\0'; i++) {
            if (!(digitos[i] >= '0' && digitos[i] <= '9')) {
                ok = 0;
            }
        }
        if (ok == 0) {
            printf("Telefone movel invalido.\n");
        }
    }
    sprintf(novo.tel.movel, "(%.2s) %.5s-%.4s",
            digitos, digitos + 2, digitos + 7);

    novo.codigo = geraCodigoCliente(novo.cpf);
    return novo;
}

void listarClientes(struct clientes cliente[3]) {
    printf("\n----- CLIENTES CADASTRADOS -----\n");
    for (int i = 0; i < 3; i++) {
        if (cliente[i].codigo != 0) {
            printf("\n[%d] %s\n", cliente[i].codigo, cliente[i].nome);
            printf("    CPF: %.0f | Sexo: %c\n", cliente[i].cpf, cliente[i].sexo);
            printf("    Fixo: %s | Movel: %s\n",
                   cliente[i].tel.fixo, cliente[i].tel.movel);
        }
    }
}

struct clientes localizaCliente(int codigo, struct clientes cliente[3]) {
    struct clientes achado = {0};
    for (int i = 0; i < 3; i++) {
        if (cliente[i].codigo == codigo) {
            achado = cliente[i];
        }
    }
    return achado;
}

/* ================= CARRINHO E VENDA ================= */

/* Soma de valor * quantidade de cada item. Posicoes nao usadas tem
   quantidade 0 e nao mudam o resultado. */
double calculaSubtotal(struct carrinhos carrinho) {
    double soma = 0;
    for (int i = 0; i < 3; i++) {
        soma = soma + carrinho.produto[i].valor * carrinho.quantidade[i];
    }
    return soma;
}

/* Com 10% de desconto sobra 90% do valor: subtotal * 90 / 100 */
double calculaTotal(double subtotal, double desconto) {
    return subtotal * (100.0 - desconto) / 100.0;
}

void listarCarrinho(struct carrinhos carrinho) {
    printf("\n======== RESUMO DA VENDA ========\n");
    printf("Cliente: %s (codigo %d)\n",
           carrinho.cliente.nome, carrinho.cliente.codigo);
    for (int i = 0; i < 3; i++) {
        if (carrinho.quantidade[i] > 0) {
            printf("\n%s - %s %s\n",
                   carrinho.produto[i].nome,
                   carrinho.produto[i].modelo.marca,
                   carrinho.produto[i].modelo.modelo);
            printf("   %d x R$ %.2f = R$ %.2f\n",
                   carrinho.quantidade[i],
                   carrinho.produto[i].valor,
                   carrinho.quantidade[i] * carrinho.produto[i].valor);
        }
    }
}

void finalizarVenda(struct carrinhos carrinho) {
    listarCarrinho(carrinho);

    /* Comeca em -1 para o while rodar. O "%%" imprime o simbolo % */
    carrinho.desconto = -1;
    while (carrinho.desconto < 0 || carrinho.desconto > 100) {
        printf("\nDesconto em porcentagem (0 a 100, ex.: 10 para 10%%): ");
        if (scanf("%lf", &carrinho.desconto) != 1) {
            scanf("%*s");
            carrinho.desconto = -1;
        }
        if (carrinho.desconto < 0 || carrinho.desconto > 100) {
            printf("Desconto fora do limite.\n");
        }
    }

    carrinho.subtotal = calculaSubtotal(carrinho);
    carrinho.total = calculaTotal(carrinho.subtotal, carrinho.desconto);
    printf("\nSubtotal......: R$ %.2f\n", carrinho.subtotal);
    printf("Desconto......: %.2f%% (R$ %.2f)\n",
           carrinho.desconto,
           carrinho.subtotal * carrinho.desconto / 100.0);
    printf("Total a pagar.: R$ %.2f\n", carrinho.total);
    printf("\nVenda concluida!\n");
}

/* Conduz a venda: escolhe o cliente, monta o carrinho e finaliza.
   O carrinho chega por valor (uma copia zerada), entao cada venda
   comeca vazia. */
void efetuarVenda(struct clientes cliente[3], struct produtos produto[10], struct carrinhos carrinho) {
    int cod;
    int qtd;
    int itens = 0;    /* tipos de produto ja no carrinho */
    int pos;          /* posicao do produto no carrinho (-1 = ainda nao esta) */
    struct produtos prod;
    printf("\n>>> NOVA VENDA <<<\n");

    /* 1) Cliente */
    listarClientes(cliente);
    printf("\nCodigo do cliente: ");
    if (scanf("%d", &cod) != 1) {
        scanf("%*s");
        cod = 0;
    }
    carrinho.cliente = localizaCliente(cod, cliente);
    if (carrinho.cliente.codigo == 0) {
        printf("\nCliente nao localizado.\n");
        return;
    }

    /* 2) Produtos: repete ate o usuario digitar 0 */
    cod = -1;
    while (cod != 0) {
        listarProdutos(produto);
        printf("\nCodigo do produto (0 para concluir): ");
        if (scanf("%d", &cod) != 1) {
            scanf("%*s");
            cod = -1;
        }

        if (cod != 0) {
            prod = localizaProduto(cod, produto);

            /* Procura o produto entre os que ja estao no carrinho */
            pos = -1;
            for (int i = 0; i < itens; i++) {
                if (carrinho.produto[i].codigo == prod.codigo) {
                    pos = i;
                }
            }

            if (prod.codigo == 0) {
                printf("\nProduto nao localizado.\n");
            } else if (pos == -1 && itens == 3) {
                printf("\nO carrinho ja tem 3 tipos de produto. Nao cabe outro.\n");
            } else {
                qtd = 0;
                while (qtd <= 0) {
                    printf("Quantidade: ");
                    if (scanf("%d", &qtd) != 1) {
                        scanf("%*s");
                        qtd = 0;
                    }
                    if (qtd <= 0) {
                        printf("Quantidade invalida.\n");
                    }
                }

                if (pos == -1) {
                    carrinho.produto[itens] = prod;
                    carrinho.quantidade[itens] = qtd;
                    itens++;
                } else {
                    carrinho.quantidade[pos] += qtd;
                    printf("\nProduto ja estava no carrinho: quantidade somada.\n");
                }
            }
        }
    }

    /* 3) Sem produtos nao ha venda */
    if (itens == 0) {
        printf("\nCarrinho vazio. Venda cancelada.\n");
        return;
    }
    finalizarVenda(carrinho);
}

/* ================= MAIN =================
   Repete o menu ate o usuario escolher 0. Os vetores comecam zerados:
   vaga vazia = codigo 0. */
int main() {
    struct clientes cliente[3] = {0};
    struct produtos produto[10] = {0};
    struct carrinhos carrinho = {0};
    struct clientes clienteNovo;
    struct produtos produtoNovo;
    int totalClientes = 0;
    int totalProdutos = 0;
    int escolha = -1;
    int jaExiste;

    while (escolha != 0) {
        mostraMenuInicial();
        /* Letra no lugar de numero: descarta o texto e vira opcao -1 */
        if (scanf("%d", &escolha) != 1) {
            scanf("%*s");
            escolha = -1;
        }

        if (escolha == 1) {
            if (totalClientes < 3) {
                clienteNovo = cadastrarCliente();

                jaExiste = 0;
                for (int i = 0; i < totalClientes; i++) {
                    if (cliente[i].codigo == clienteNovo.codigo) {
                        jaExiste = 1;
                    }
                }

                if (clienteNovo.codigo == 0) {
                    /* CPF comecando com 000000 geraria codigo 0,
                       que e o valor de vaga vazia */
                    printf("\nCPF nao aceito. Cadastro cancelado.\n");
                } else if (jaExiste == 1) {
                    printf("\nJa existe cliente com esse codigo. Cadastro cancelado.\n");
                } else {
                    cliente[totalClientes] = clienteNovo;
                    totalClientes++;
                    printf("\nCliente cadastrado! Codigo: %d\n", clienteNovo.codigo);
                }
            } else {
                printf("\nLimite de 3 clientes atingido.\n");
            }
        } else if (escolha == 2) {
            if (totalProdutos < 10) {
                produtoNovo = cadastrarProduto();

                jaExiste = 0;
                for (int i = 0; i < totalProdutos; i++) {
                    if (produto[i].codigo == produtoNovo.codigo) {
                        jaExiste = 1;
                    }
                }

                if (jaExiste == 1) {
                    printf("\nJa existe produto com esse codigo. Cadastro cancelado.\n");
                } else {
                    produto[totalProdutos] = produtoNovo;
                    totalProdutos++;
                    printf("\nProduto cadastrado! Codigo: %d\n", produtoNovo.codigo);
                }
            } else {
                printf("\nLimite de 10 produtos atingido.\n");
            }
        } else if (escolha == 3) {
            listarClientes(cliente);
        } else if (escolha == 4) {
            listarProdutos(produto);
        } else if (escolha == 5) {
            efetuarVenda(cliente, produto, carrinho);
        } else if (escolha != 0) {
            printf("\nOpcao invalida.\n");
        }
    }
    printf("\nPrograma encerrado.\n");
    return 0;
}