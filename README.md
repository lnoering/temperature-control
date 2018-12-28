## Controle de Temperatura

### Plataforma
    - Arduino

### Objetivo
    Controlar a temperatura de um freezer, para o processo de fermentação da cerveja.


### Melhorias a serem feitas
    - Fazer o "Set Point Temperatura" aceitar valor float.
    - Colocar um cronometro na tela.
    - Criar opção para visualizar em Fahrenheit.
    

### Menu
> "Set Point Temperatura"
-   Setar a temperatura que manterá a saída acionada.

> "Offset Rele"
-   Tempo em segundo para acontecer mudança do status da saída. (Prevenir mundanças consecutivas, para não queimar o motor).

> "Formato Temperatura"
-   (C|F) Sendo C = Celsius e F = Fahrenheit

> "Offset Temperatura"
-   Decrementar diretamente da leitura do sensor.

> "Salvar e Sair"
-   Grava na EEPROM e sai do menu.

> "Sair"
-   Apenas sai do Menu.