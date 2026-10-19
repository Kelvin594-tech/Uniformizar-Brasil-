/* Biblioteca para entrada/saida e manipulacao de arquivos. */
#include <stdio.h>

/* Quantidade de tipos de produtos oferecidos. */
#define TOTAL_PRODUTOS 5
#define ESTOQUE_INICIAL 500000.0f

/* Lista de produtos disponiveis para cadastro. */
const char *produtos[TOTAL_PRODUTOS] = {
    "Camiseta", "Calca", "Moletom", "Avental", "Jaleco"
};

/* Consumo estimado de tecido por peca, em metros.
   Valores ficticios para demonstracao. */
const float consumo[TOTAL_PRODUTOS] = {
    1.2f, 1.6f, 2.0f, 0.8f, 2.0f
};

/* Estrutura que guarda os dados de cada pedido. */
typedef struct {
    int numero;
    char nome[50];
    char telefone[20];
    int quantidade[TOTAL_PRODUTOS];
    char tamanho[TOTAL_PRODUTOS][10];
    float tecidoUtilizado;
    int concluido;
} Pedido;

/* Funcoes auxiliares implementadas usando apenas recursos de stdio.h. */
/* Remove a quebra de linha do texto digitado ou lido do arquivo. */
void removerQuebraLinha(char texto[]) {
    int i = 0;
    while (texto[i] != '\0') {
        if (texto[i] == '\n' || texto[i] == '\r') {
            texto[i] = '\0';
            return;
        }
        i++;
    }
}

/* Copia texto respeitando o limite do vetor de destino. */
void copiarTexto(char destino[], const char origem[], int limite) {
    int i = 0;
    if (limite <= 0) return;
    while (origem[i] != '\0' && i < limite - 1) {
        destino[i] = origem[i];
        i++;
    }
    destino[i] = '\0';
}

/* Converte texto para numero inteiro. */
int converterInteiro(const char texto[]) {
    int valor = 0;
    sscanf(texto, "%d", &valor);
    return valor;
}

/* Converte texto para numero decimal. */
float converterReal(const char texto[]) {
    float valor = 0.0f;
    sscanf(texto, "%f", &valor);
    return valor;
}

/* Le texto sem deixar a quebra de linha no final. */
/* Solicita e le um texto informado pelo usuario. */
void lerTexto(char mensagem[], char destino[], int tamanho) {
    printf("%s", mensagem);

    if (fgets(destino, tamanho, stdin) != NULL) {
        removerQuebraLinha(destino);
    } else {
        destino[0] = '\0';
    }
}

/* Le um numero inteiro e valida a entrada. */
/* Le um inteiro e valida a entrada antes de retornar o valor. */
int lerInteiro(char mensagem[]) {
    char linha[100];
    char extra;
    int valor;

    while (1) {
        printf("%s", mensagem);

        if (fgets(linha, sizeof(linha), stdin) == NULL) {
            return -1;
        }

        if (sscanf(linha, " %d %c", &valor, &extra) == 1) {
            return valor;
        }

        printf("Digite um numero inteiro valido.\n");
    }
}

/* Inicializa o estoque apenas se o arquivo nao existir. */
/* Cria o estoque inicial somente quando estoque.txt ainda nao existe. */
int inicializarEstoque() {
    FILE *arquivo = fopen("estoque.txt", "r");

    if (arquivo != NULL) {
        fclose(arquivo);
        return 1;
    }

    arquivo = fopen("estoque.txt", "w");

    if (arquivo == NULL) {
        return 0;
    }

    fprintf(arquivo, "%.2f\n", ESTOQUE_INICIAL);
    fclose(arquivo);

    return 1;
}

/* Le e retorna a quantidade de tecido registrada no arquivo. */
float consultarEstoque() {
    FILE *arquivo;
    float quantidade;

    arquivo = fopen("estoque.txt", "r");

    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo de estoque.\n");
        return -1.0f;
    }

    if (fscanf(arquivo, "%f", &quantidade) != 1) {
        fclose(arquivo);
        printf("Erro ao ler o estoque.\n");
        return -1.0f;
    }

    fclose(arquivo);
    return quantidade;
}

/* Salva o saldo atualizado do estoque no arquivo. */
int salvarEstoque(float quantidade) {
    FILE *arquivo = fopen("estoque.txt", "w");

    if (arquivo == NULL) {
        return 0;
    }

    fprintf(arquivo, "%.2f\n", quantidade);
    fclose(arquivo);

    return 1;
}

/* Gera numeros sequenciais para os pedidos. */
/* Gera o proximo numero de identificacao e atualiza contador.txt. */
int gerarNumeroPedido() {
    FILE *arquivo;
    int numero = 1000;

    arquivo = fopen("contador.txt", "r");

    if (arquivo != NULL) {
        if (fscanf(arquivo, "%d", &numero) != 1) {
            fclose(arquivo);
            return -1;
        }

        fclose(arquivo);
    }

    numero++;

    arquivo = fopen("contador.txt", "w");

    if (arquivo == NULL) {
        return -1;
    }

    fprintf(arquivo, "%d\n", numero);
    fclose(arquivo);

    return numero;
}

/* Salva todos os itens do pedido em uma unica linha. */
/* Grava os dados do pedido no final de pedidos.txt. */
int salvarPedido(Pedido pedido) {
    FILE *arquivo = fopen("pedidos.txt", "a");
    int i;

    if (arquivo == NULL) {
        return 0;
    }

    fprintf(arquivo, "%d|%s|%s",
            pedido.numero, pedido.nome, pedido.telefone);

    for (i = 0; i < TOTAL_PRODUTOS; i++) {
        fprintf(arquivo, "|%d|%s",
                pedido.quantidade[i], pedido.tamanho[i]);
    }

    fprintf(arquivo, "|%.2f|%d\n",
            pedido.tecidoUtilizado, pedido.concluido);

    fclose(arquivo);
    return 1;
}

/* Interpreta uma linha de pedido, preservando campos vazios. */
/* Separa e valida os campos de um registro, preservando campos vazios. */
int interpretarLinhaPedido(char linha[], Pedido *pedido) {
    char *campos[15];
    char *p;
    int i;

    removerQuebraLinha(linha);
    campos[0] = linha;

    for (i = 1; i < 15; i++) {
        p = campos[i - 1];
        while (*p != '\0' && *p != '|') p++;
        if (*p != '|') return -1;
        *p = '\0';
        campos[i] = p + 1;
    }

    p = campos[14];
    while (*p != '\0') {
        if (*p == '|') return -1;
        p++;
    }

    if (sscanf(campos[0], "%d", &pedido->numero) != 1) return -1;
    copiarTexto(pedido->nome, campos[1], sizeof(pedido->nome));
    copiarTexto(pedido->telefone, campos[2], sizeof(pedido->telefone));

    for (i = 0; i < TOTAL_PRODUTOS; i++) {
        if (sscanf(campos[3 + i * 2], "%d", &pedido->quantidade[i]) != 1)
            return -1;
        copiarTexto(pedido->tamanho[i], campos[4 + i * 2], sizeof(pedido->tamanho[i]));
    }

    if (sscanf(campos[13], "%f", &pedido->tecidoUtilizado) != 1) return -1;
    if (sscanf(campos[14], "%d", &pedido->concluido) != 1) return -1;
    return 1;
}

/* Le uma linha do arquivo. Retorna -1 se a linha estiver incompleta. */
/* Le uma linha do arquivo e transforma os dados em um Pedido. */
int lerPedido(FILE *arquivo, Pedido *pedido) {
    char linha[600];
    if (fgets(linha, sizeof(linha), arquivo) == NULL) return 0;
    return interpretarLinhaPedido(linha, pedido);
}

/* Mostra os produtos e as quantidades de um pedido. */
/* Exibe os dados, os itens e o status de um pedido. */
void mostrarPedido(Pedido pedido) {
    int i;

    printf("\n--- DADOS DO PEDIDO ---\n");
    printf("Numero: %d\n", pedido.numero);
    printf("Cliente: %s\n", pedido.nome);
    printf("Telefone: %s\n", pedido.telefone);

    printf("\nItens do pedido:\n");

    for (i = 0; i < TOTAL_PRODUTOS; i++) {
        if (pedido.quantidade[i] > 0) {
            printf("%s: %d peca(s), tamanho %s\n",
                   produtos[i], pedido.quantidade[i],
                   pedido.tamanho[i]);
        }
    }

    printf("Tecido reservado: %.2f metros\n",
           pedido.tecidoUtilizado);

    if (pedido.concluido == 1) {
        printf("Status: Concluido, pronto para expedicao.\n");
    } else {
        printf("Status: Em producao.\n");
    }
}

/* 1 - Cadastro com varios produtos no mesmo pedido. */
/* CADASTRO DO CLIENTE E DO PEDIDO: coleta os dados e os produtos solicitados. */
void cadastrarPedido() {
    Pedido pedido = {0};
    float estoque;
    float total = 0.0f;
    int i, totalPecas = 0;

    printf("\n--- CADASTRO DE PEDIDO ---\n");

    /* Coleta nome e telefone para identificar o cliente. */
    lerTexto("Nome do cliente: ",
             pedido.nome, sizeof(pedido.nome));

    lerTexto("Telefone: ",
             pedido.telefone, sizeof(pedido.telefone));

    /* Recebe as quantidades e tamanhos e calcula o consumo de tecido. */
    printf("\nInforme a quantidade de cada produto.\n");
    printf("Digite 0 para os produtos que nao deseja.\n");

    for (i = 0; i < TOTAL_PRODUTOS; i++) {
        printf("\nProduto: %s\n", produtos[i]);

        pedido.quantidade[i] = lerInteiro("Quantidade: ");

        if (pedido.quantidade[i] < 0) {
            printf("Quantidade invalida. Cadastro cancelado.\n");
            return;
        }

        if (pedido.quantidade[i] > 0) {
            lerTexto("Tamanho (ex.: P, M, G ou GG): ",
                     pedido.tamanho[i],
                     sizeof(pedido.tamanho[i]));

            total += pedido.quantidade[i] * consumo[i];
            totalPecas += pedido.quantidade[i];
        } else {
            copiarTexto(pedido.tamanho[i], "-", sizeof(pedido.tamanho[i]));
        }
    }

    if (totalPecas == 0) {
        printf("O pedido precisa ter pelo menos um produto.\n");
        return;
    }

    /* Verifica se ha tecido suficiente para atender ao pedido. */
    estoque = consultarEstoque();

    if (estoque < 0) {
        return;
    }

    printf("\nTotal de pecas: %d\n", totalPecas);
    printf("Tecido necessario: %.2f metros\n", total);
    printf("Estoque disponivel: %.2f metros\n", estoque);

    if (total > estoque) {
        printf("Estoque insuficiente. Pedido nao cadastrado.\n");
        printf("Faltam %.2f metros de tecido.\n", total - estoque);
        return;
    }

    /* GERA O NUMERO DE IDENTIFICACAO DO PEDIDO. */
    pedido.numero = gerarNumeroPedido();

    if (pedido.numero < 0) {
        printf("Erro ao gerar numero do pedido.\n");
        return;
    }

    pedido.tecidoUtilizado = total;
    pedido.concluido = 0;

    if (!salvarPedido(pedido)) {
        printf("Erro ao salvar o pedido.\n");
        return;
    }

    if (!salvarEstoque(estoque - total)) {
        printf("ATENCAO: pedido salvo, mas o estoque nao foi atualizado.\n");
        printf("Verifique os arquivos antes de continuar.\n");
        return;
    }

    printf("\nPedido cadastrado com sucesso!\n");
    printf("Numero do pedido: %d\n", pedido.numero);
    printf("Tecido reservado: %.2f metros\n", total);
    printf("Estoque restante: %.2f metros\n", estoque - total);
}

/* 2 - Consulta o saldo salvo do estoque. */
/* CONSULTA DE ESTOQUE: mostra o saldo atual de tecido. */
void controlarEstoque() {
    float estoque = consultarEstoque();

    if (estoque >= 0) {
        printf("\n--- CONTROLE DE ESTOQUE ---\n");
        printf("Tecido disponivel: %.2f metros\n", estoque);
    }
}

/* 3 - Atualiza o status sem apagar linhas antigas incompletas. */
/* PROCESSO DE PRODUCAO: localiza um pedido e registra sua conclusao. */
void processoProducao() {
    FILE *arquivo, *temporario;
    Pedido pedido;
    char linha[600], copia[600];
    int numero, resultado, encontrado = 0, alterado = 0;
    int i;

    printf("\n--- PROCESSO DE PRODUCAO ---\n");
    numero = lerInteiro("Numero do pedido: ");
    if (numero <= 0) {
        printf("Numero invalido.\n");
        return;
    }

    arquivo = fopen("pedidos.txt", "r");
    if (arquivo == NULL) {
        printf("Nenhum pedido cadastrado.\n");
        return;
    }

    temporario = fopen("temporario.txt", "w");
    if (temporario == NULL) {
        fclose(arquivo);
        printf("Erro ao atualizar o pedido.\n");
        return;
    }

    while (fgets(linha, sizeof(linha), arquivo) != NULL) {
        copiarTexto(copia, linha, sizeof(copia));
        resultado = interpretarLinhaPedido(copia, &pedido);

        if (resultado != 1) {
            /* Mantem linhas antigas incompletas exatamente como estavam. */
            fputs(linha, temporario);
            continue;
        }

        if (pedido.numero == numero) {
            encontrado = 1;
            if (pedido.concluido == 1) {
                printf("Este pedido ja foi concluido.\n");
            } else {
                printf("\nEtapas do processo de producao:\n");
                printf("1. Corte das pecas\n2. Costura\n");
                printf("3. Estamparia, quando necessaria\n");
                printf("4. Conferencia de quantidade, tamanhos e qualidade\n");
                printf("5. Empacotamento\n");
                pedido.concluido = 1;
                alterado = 1;
                printf("\nPedido marcado como concluido.\n");
            }

            fprintf(temporario, "%d|%s|%s", pedido.numero, pedido.nome, pedido.telefone);
            for (i = 0; i < TOTAL_PRODUTOS; i++)
                fprintf(temporario, "|%d|%s", pedido.quantidade[i], pedido.tamanho[i]);
            fprintf(temporario, "|%.2f|%d\n", pedido.tecidoUtilizado, pedido.concluido);
        } else {
            fputs(linha, temporario);
        }
    }

    fclose(arquivo);
    if (fclose(temporario) != 0) {
        printf("Erro ao gravar arquivo temporario.\n");
        return;
    }

    if (!encontrado) {
        remove("temporario.txt");
        printf("Pedido nao encontrado entre os registros validos.\n");
        return;
    }

    if (alterado) {
        /* Nao remove o original antes de confirmar que o temporario existe. */
        if (remove("pedidos.txt") != 0) {
            printf("Nao foi possivel substituir pedidos.txt. O original foi mantido.\n");
            return;
        }
        if (rename("temporario.txt", "pedidos.txt") != 0) {
            printf("Erro ao renomear o arquivo temporario. Verifique temporario.txt.\n");
            return;
        }
        printf("Status atualizado: pronto para entrega.\n");
    } else {
        remove("temporario.txt");
    }
}

/* 4 - Consulta pedido e ignora registros antigos incompletos. */
/* CONSULTA DO PEDIDO: procura pelo numero de identificacao informado. */
void consultarPedido() {
    FILE *arquivo;
    Pedido pedido;
    int numero, resultado, encontrado = 0;

    printf("\n--- CONSULTA DE PEDIDO ---\n");
    numero = lerInteiro("Digite o numero do pedido: ");
    if (numero <= 0) {
        printf("Numero invalido.\n");
        return;
    }

    arquivo = fopen("pedidos.txt", "r");
    if (arquivo == NULL) {
        printf("Nenhum pedido cadastrado.\n");
        return;
    }

    while ((resultado = lerPedido(arquivo, &pedido)) != 0) {
        /* Pula registros incompletos e continua procurando. */
        if (resultado == -1) continue;

        if (pedido.numero == numero) {
            mostrarPedido(pedido);
            encontrado = 1;
            break;
        }
    }

    fclose(arquivo);
    if (!encontrado) printf("Pedido nao encontrado entre os registros validos.\n");
}

/* Menu principal. */
/* MENU PRINCIPAL: permite acessar as funcoes do sistema. */
int main() {
    int opcao;

    if (!inicializarEstoque()) {
        printf("Nao foi possivel inicializar o estoque.\n");
        return 1;
    }

    /* Mantem o menu em execucao ate o usuario escolher Sair. */
    do {
        printf("\n=== UNIFORMIZAR BRASIL ===\n");
        printf("1 - Cadastrar pedido com varios produtos\n");
        printf("2 - Consultar estoque de tecido\n");
        printf("3 - Registrar conclusao da producao\n");
        printf("4 - Consultar status do pedido\n");
        printf("5 - Sair\n");

        opcao = lerInteiro("Escolha uma opcao: ");

        /* Encaminha a escolha do usuario para a funcionalidade correspondente. */
        switch (opcao) {
            case 1:
                cadastrarPedido();
                break;

            case 2:
                controlarEstoque();
                break;

            case 3:
                processoProducao();
                break;

            case 4:
                consultarPedido();
                break;

            case 5:
                printf("Programa encerrado.\n");
                break;

            default:
                printf("Opcao invalida.\n");
        }

    } while (opcao != 5);

    return 0;
}
