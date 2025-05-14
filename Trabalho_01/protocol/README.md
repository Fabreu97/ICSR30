# 📄 Mini-RFC: Protocolo de Transferência Confiável — Fernando Protocol (v1.0)

## 1. Visão Geral

O **Fernando Protocol** é um protocolo confiável de transferência de arquivos construído sobre o UDP. Ele implementa controle de conexão, controle de fluxo e verificação de ordem de pacotes utilizando números de sequência e pacotes de confirmação (`ACK`). Seu funcionamento é similar ao TCP, mas com um fluxo adaptado ao controle do lado do cliente.

---

## 2. Tipos de Pacotes

Cada pacote contém os seguintes campos principais:

- `flags`: tipo do pacote (`SYN`, `SYN_ACK`, `ACK`, `SND`, `FIN`, `FIN_ACK`)
- `seq_number`: número da sequência (aplicado principalmente em `SND`)
- `ack`: número do próximo pacote esperado (aplicado em `ACK`)
- `length`: tamanho do campo `data`
- `data`: dados úteis (até `LENGTH` bytes)

---

## 3. Handshake (Estabelecimento de Conexão)

1. **Cliente → Servidor**: envia `SYN`
2. **Servidor → Cliente**: responde com `SYN_ACK`
3. **Cliente → Servidor**: envia `ACK(0)` — indicando que espera o pacote com `seq_number = 0`

---

## 4. Transferência de Dados

### 4.1. Fluxo Padrão

- O servidor envia pacotes `SND(seq=X)` com os dados do arquivo.
- O cliente verifica:
  - Se `recv_packet.seq_number == ack_client`, ele grava os dados no arquivo, incrementa `ack_client` e envia `ACK(ack_client)`.
  - Caso contrário, ignora o pacote e **reitera o último ACK**.

### 4.2. Reenvio

- Se o cliente não recebe um pacote dentro de `timeout`, ele **reenvia o último ACK**.
- O servidor, ao receber um ACK repetido, entende que o cliente ainda espera o mesmo pacote e **reenvia o `SND(seq=ack_client)`**.

---

## 5. Encerramento

1. **Servidor → Cliente**: envia `FIN` após o último pacote
2. **Cliente → Servidor**: responde com `FIN_ACK` e fecha o arquivo
3. Conexão é encerrada

---

## 6. Controle de Timeout

- O cliente ajusta dinamicamente o tempo de espera (`timeout`) com base no tempo de ida e volta (RTT) estimado entre envio do ACK e recepção do SND.

---

## 7. Segurança e Confiabilidade

- O protocolo garante entrega **confiável** e **em ordem**:
  - Nenhum pacote fora de ordem é aceito.
  - Reenvio é feito quando há perda.
- Detecção de fim é explícita via `FIN`.

---

## 8. Estrutura de Pacote (Exemplo em C)

```c
#define LENGTH 512

typedef struct {
    uint8_t flags;         // Tipo do pacote: SYN, SND, ACK etc.
    uint16_t seq_number;   // Número da sequência
    uint16_t ack;          // Número do próximo pacote esperado
    uint16_t length;       // Tamanho dos dados
    char data[LENGTH];     // Dados
} Packet;
```