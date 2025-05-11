# TRABALHO 1 - Implementação de Transferência de Arquivos Confiável sobre UDP com Sockets

## Objetivo do Projeto:

Desenvolver uma aplicação cliente-servidor para transferência de arquivos utilizando o protocolo UDP. O foco principal é a implementação de mecanismos básicos de controle e confiabilidade diretamente sobre UDP, simulando funcionalidades que o TCP oferece nativamente.

## Requisitos Gerais:

- Linguagem C
    - Desvantagem de usar C:
        - Não tem funcionalidade de tratamento de erros em um nível de abstração equivalente ao mecanismo **try/except** presente em linguagens como Python
- Bibliotecas utilizadas para comunicação via socket:
    - sys/socket.h => socket()
    - netinet/in.h => struct sockaddr_in
    - unistd.h     => close()

## Requisitos do Servidor UDP:

## Design do Protocolo:

### Segmentação e Tamanho do Buffer:
Como será o arquivo?

Pacote de dados é dividido em 2 partes:

    - Header(16B)
        - sequência do número do pacote(4B)
        - ACK(4B)
        - FLAGS para saber o tipo de pacote recebido(4B)
        - LENGTH = tamanho do vetor de dados(4B)
    - Payload(1008B)
        - char[1008]

##### Qual o tamanho máximo de dados por datagrama UDP?

**Resposta:** Tamanho será de 1008 Bytes.

##### Este tamanho deve ser fixo ou variável?Como ele se relaciona com o MTU (Maximum Transmission Unit) da rede?

**Resposta:** Deve ser variável para que dados com tamanhos não multiplos ao PAYLOAD dos pacotes sejam transferidos, porém deve ser limitado que no máximo tenha o mínimo MTU entre as duas máquinas que estão se comunicando.Na verdade é limitado ao menor MTU de todas as máquinas que estão no caminho do envio da informação, caso encontre PAYLOAD maior que MTU de uma máquina o pacote é fragmentando em outros pacotes menores.

##### Os buffers de envio (servidor) e recepção (cliente) precisam ter tamanhos relacionados?

**Resposta:**

## Referência das informações

https://www.cloudflare.com/pt-br/learning/network-layer/what-is-mtu/#:~:text=Em%20uma%20rede%2C%20a%20unidade,passagens%20sob%20rodovias%20ou%20t%C3%BAneis.

