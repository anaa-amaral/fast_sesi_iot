# Projeto SA - Fast Sesi

<p>
  <img src="https://raw.githubusercontent.com/hisabellippel/fast-sesi/refs/heads/main/asets/imagens/barraAbaixo/logo.png" width="100" alt="accessibility text">
</p>

## Integrantes
Ana Luiza do Amaral, Gustavo Baumann, Hisabel Lippel e Julia Izabel

## Sobre
Nesse projeto após a conclusão dos mockups, recriamos a aplicação, agora estamos conectando o nosso aplicativo a matéria de IOT, para a configuração dos sensores.
No momento estamos na quarta etapa de nosso projeto, iniciando as aplicações em C++.

Esse repositório contém um sistema de uma ferrovia onde o Administrador pode cadastrar novos usuários e excluílos, porém o usuário não pode cadastrar e excluir demais funcionários, além a capacitação de sensores. É um trabalho para a SA - da matéria de Desenvolvimento de Sistemas + Banco de Dados + IOT + Programação de aplicativos.

## Funcionalidades
- Configurar o ESP32 com SSID;
- Configurar senha da rede Wi-Fi;
- Exibir no monitor serial o status da conexão e o endereço IP obtido;
- Garantir reconexão automática em caso de falha;
- Teste de conexão Wi-Fi exibindo IP no monitor serial.

## Como conectar com o Banco de Dados
Antes de utilizar o sistema, é importante verificar se as variáveis no arquivo `db.php` estão corretas para o seu servidor. Modifique principalmente as linhas a seguir com o usuário, senha e porta do MySql adequadas.

```
  $username = "João";
  $password = "6768";
  $credencial = "1234"
```
## Script SQL

Execute o arquivo `db.sql` no banco de dados para criar o banco e adicionar um usuário e um cliente que será utilizado em operações básicas do sistema.

> [!IMPORTANT]
> Sem os dados corretos, podem ocorrer erros ao acessar as páginas que utilizam conexão com banco de dados.
> O banco de dados deve ser inserido na sua máquina para conseguir navegar dentro do App.

## Evoluçaõ do projeto

Neste quadro Kanban acima, você pode ver nossa evolução como equipe. Dentro dele está separados as principáis tarefas e seus membros atribuídos.