# Sistema de Inscrição e Presença para Evento (Cliente–Servidor em C)

## Visão Geral

Este projeto implementa um sistema simples de **inscrição e controle de presença** para um evento evangelístico, usando a arquitetura **cliente–servidor** em C.

- O **servidor** é responsável por armazenar os participantes em memória e em arquivo texto (`participantes.txt`) e processar comandos enviados por clientes via **TCP**.
- O **cliente** é uma aplicação em C (modo texto) que apresenta um menu para o usuário e se comunica com o servidor através de sockets.

---

## Arquitetura do Projeto

- **Linguagem:** C
- **Modelo:** Cliente–Servidor, usando **sockets TCP**
- **Servidor:**
  - Aceita conexões na porta `5000`;
  - Processa um comando por conexão;
  - Usa arquivo texto (`participantes.txt`) para armazenar os dados.

- **Cliente:**
  - Aplicação em C, modo texto (terminal);
  - Apresenta um menu com opções para o usuário;
  - Para cada operação, abre uma conexão TCP com o servidor, envia o comando e exibe a resposta.

### Estrutura de Diretórios (Checkpoint 2)

```text
projeto-c-caminho-c/
├─ README.md
├─ server/
│  ├─ server.c
│  ├─ participants.c
│  ├─ participants.h
│  └─ participantes.txt
└─ client/
   └─ client.c
```

---

## Protocolo de Comunicação (Texto)

A comunicação entre cliente e servidor acontece por **linhas de texto** terminadas em `\n`.

### 1. Adicionar participante

**Comando enviado ao servidor:**

```text
ADD|NOME DO PARTICIPANTE|TELEFONE
```

Exemplo:

```text
ADD|João da Silva|99999-9999
```

**Resposta (sucesso):**

```text
OK|<id>
```

Exemplo:

```text
OK|1
```

**Resposta (erro):**

```text
ERR|mensagem de erro
```

---

### 2. Listar participantes

**Comando:**

```text
LIST
```

**Resposta:**

Várias linhas do tipo:

```text
ITEM|id|nome|telefone|checked_in
...
END
```

- `checked_in` = `0` (não fez check-in) ou `1` (presente).

Exemplo:

```text
ITEM|1|João da Silva|99999-9999|0
ITEM|2|Maria Souza|88888-8888|1
END
```

---

### 3. Fazer check-in

**Comando:**

```text
CHECKIN|id
```

Exemplo:

```text
CHECKIN|2
```

**Resposta (sucesso):**

```text
OK
```

**Resposta (erro):**

```text
ERR|Participante nao encontrado
```

---

### 4. Estatísticas (implementado no servidor)

**Comando:**

```text
STATS
```

**Resposta:**

```text
STATS|totalInscritos|totalPresentes
```

> No Checkpoint 2, este comando já está implementado no servidor e pode ser testado via `nc`/`telnet`.  
> A opção correspondente no cliente (menu "4 - Ver estatísticas") ainda está em desenvolvimento.

---

## Como Compilar

### Servidor

```bash
cd server
gcc server.c participants.c -o server_app
```

### Cliente

```bash
cd client
gcc client.c -o client_app
```

---

## Como Executar

1. **Iniciar o servidor**

   Na pasta `server/`:

   ```bash
   ./server_app
   ```

   O servidor ficará escutando na porta `5000`.

2. **Iniciar o cliente**

   Em outro terminal, na pasta `client/`:

   ```bash
   ./client_app
   ```

   O programa mostrará um menu como:

   ```text
   ==== SISTEMA DE INSCRICOES (CLIENTE) ====
   1 - Cadastrar participante
   2 - Listar participantes
   3 - Fazer check-in
   4 - Ver estatisticas (em desenvolvimento)
   0 - Sair
   Escolha uma opcao:
   ```

---

## Status por Checkpoint

### Checkpoint 1 (já concluído)

- Definição da arquitetura cliente–servidor em C;
- Definição do protocolo de texto (`ADD`, `LIST`, `CHECKIN`, `STATS`);
- Implementação do servidor TCP em C:
  - Aceita conexões na porta 5000;
  - Comando `ADD`: cadastra participante em memória e grava no arquivo;
  - Comando `LIST`: lista participantes, lendo do array em memória;
- Módulo de gerenciamento de participantes (`participants.c` / `.h`);
- Persistência em arquivo texto (`participantes.txt`).

### Checkpoint 2 (parcialmente concluído)

- Implementação do **cliente em C** com menu no terminal;
- Cliente integra com o servidor usando os comandos:
  - `ADD` (Opção 1 – Cadastrar participante)
  - `LIST` (Opção 2 – Listar participantes)
  - `CHECKIN` (Opção 3 – Fazer check-in)
- Servidor atualizado com:
  - Implementação do comando `CHECKIN|id`;
  - Implementação do comando `STATS` (estatísticas básicas).

**Ponto pendente (ainda não implementado no CP2):**

- A opção **4 - Ver estatísticas** no cliente ainda não consome o comando `STATS` do servidor.  
  No momento, ela apenas exibe uma mensagem de “Em desenvolvimento”.

Esse ponto será finalizado na versão final do projeto.
