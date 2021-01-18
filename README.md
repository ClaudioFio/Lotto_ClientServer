# The Lottery !!

This is a distributed client-server application that reproduces the game of Lottery. The application must allow various client hosts (players) to make plays and send them to a
remote server, whose task is to store the bets, make the draws and process the winnings, notifying the amount to the players.

The game of Lotto (or simply Lotto) is a gambling game that consists in the extraction of five numbers between 1 and 90, with a prize for those who guess at least one.<br>
Each draw involves eleven wheels:  *Bari, Cagliari, Firenze, Genova, Milano, Napoli, Palermo, Roma, Torino, Venezia* and *Nazionale*.

In each draw, five numbers between 1 and 90 are drawn for each wheel, without re-insertion. The game consists of betting on the numbers that will be drawn on the various wheel.<br>
The order in which they are extracts is not significant. You can bet on the release of one or more numbers on a wheel, on multiple wheels or on all wheels.<br>

In particular, given a wheel, we talk about the type of bet:
- **Estratto**, if you bet on the draw out of only one number;<br>
- **Ambo**, if you bet on the simultaneous draw out of two numbers;<br>
- **Terno**, if you bet on the simultaneous draw out of three numbers;<br>
- **Quaterno**, if you bet on the simultaneous draw out of four numbers;<br>
- **Cinquina**, if you bet on the simultaneous draw out of five numbers.<br>

The winning amount is fixed and depends on how many numbers have been guessed, on what has been played (Ambo, Terno, etc.), and how many numbers have been played.

| Number played     | Number guessed | Winning amount (€)     |
| :---:        |    :----:   |          :---: |
| 1      | 1       | 11,23   |
| 2   | 2        | 250      |
| 3      | 3       | 4.500   |
| 4   | 4        | 120.000      |
| 5      | 5       | 6.000.000   |

In the same bet, it is possible to combine more types of bets. A player could for example play the numbers 5, 18 and 33, betting € 1 on the *Terno* and € 2 on *Ambo*. If a *Terno* comes out of the five numbers drawn, the player wins 4500 €. If both are rolled, the player wins an amount equal to € 250 / k, where k is the number of *Ambo* generated by numbers 5, 18, and 33. If more than one come out, the previous amount is multiplied by the number of *Ambo* drow out.<br>
If more wheels are indicated in the same ticket, the previous considerations continue to apply, but the amounts are equally spread over the chosen wheels. So, for example, player could do the same play described above, but this time indicate two wheels: *Roma* and *Firenze*. In this case, the winnings amount would be divided by two, so the *Terno* drow out on one of the two wheels is paid € 2250, while the *Ambo* are paid each 250 / 2k €.<br>
Finally, if Player makes the bet on all of the wheels, the winnings are divided by 11 (total number of wheels). So, if the *Terno* comes out on *Napoli* and two *Ambo* (for example one on *Bari* and the other on the *Nazione*) player wins € 4500/11 for the terno, plus € 250 / 11k for each of the two *Ambo*.

## Server
The server must allow clients to make bets for the next draw scheduled, view the latest bets and winnings.<br>
The server will need to be on able to handle concurrent requests, through the multi-process approach.<br>

The server is run as follows: <br>
`./lotto_server <port> <period>`<br>
where:<br>
`<port>`   is the port the server is listening on. The server must be reachable from all IP addresses of the machine it is running on;<br>
`<period>` optional parameter that specifies the time between one extraction and the next. If you don't specify the parameter, the extractions are performed every 5 minutes.<br>

#### Server's command
`!signup username password` register a new user characterized by username and password. When the command is successful, the server creates a log file for the new user, where it will subsequently store the bets and the winnings made.<br>

`!login username password` authenticates a user. the server generates a string of 10 alphanumeric random characters (session id), associate it with the authenticated username. If the authentication with username and password fails, the server must make two further attempts available to the client, which failed to close the connection. The server must block the IP address for 30 minutes.<br>

`!invia_giocata lottery_ticket` receives a new ticket from the user. Once done this notifies the client that the game has been made.<br>

`!vedi_giocate type` sends information on the bets made to the client. If **type** is 0, the server sends the past bets of the client; if **type** is 1, the server sends the active bets, those awaiting the next draw.<br>

`!vedi_estrazione n wheel` sends to the client the numbers drawn in the last *n* draws, on the received wheel. If the wheel was not specified, the server sends all wheel information.<br>

`!vedi_vincite` reads the customer's winnings and sends a balance sheet to the customer.<br>

`!esci` the server invalidates the session id and sends a message to the client that logout has occurred, closes the socket TCP and exit<br>

## Client
The client is run as follows: <br>
`./lotto_client <IP server> <server port>`<br>
where:<br>
`<IP server>`  is the address of the host on which the server is running;<br>
`<server port>` is the port on which the server is listening.<br>

#### Client's command
`!help command` shows the details of the command specified as a parameter. If the command is not specified it returns a brief description of all commands.<br>

`!signup username password` register a new user characterized by username and password. If it is already present another user with the same username, it must return an error. If the command is successful, the server creates a log file for the new user.<br>

`!login username password` sends authentication credentials to the server. If they are valid, it receives a session id from the server. If the client sends invalid credentials, it receives an error message from the server.<br>

`!invia_giocata lottery_ticket` sends a play to the server. The play is described by the ticket, which contains the chosen wheels, the numbers and the amounts for each type of bet.<br>
An example of this command's format is `> !invia_giocata –r Roma Milano –n 15 19 33 –i 0 5 10`<br>

`!vedi_giocate type` requests from the server the bets that the client made. If **type** is 0, the client receive his past bets; if **type** is 1, the client receive his active bets, those awaiting the next draw.<br>

`!vedi_estrazione n wheel` requests from the server the numbers drawn in the last n draws, on the wheel specified as a parameter. If the wheel is not specified, the server sends the drawn numbers on all wheels.<br>

`!vedi_vincite` requests from the server all the client's winnings, the draw in which they were made and a final balance for type of play.<br>

`!esci` invia un messaggio di logout al server, closes the socket TCP and exit.<br>


## Files

The server organise the client's files and the server's files in the */txt* directory as follow:

#### /txt/clients.txt
The clients file's format is: *`username`* *`password`* *`sessionID`* <br>
where:<br>
*sessionID* is a string of 10 alphanumeric random characters if the client is "online", or it contains the *offline* value if the client is not connected.<br>
Eg. <br>
`pippo pippo offline`<br>
`pluto pluto 7igPivQaZb`

#### /txt/*username*.txt
The client's file format is: *`timestamp`* *`-r`* *`Betted_Wheels_Res`* *`Betted_Wheels`* *`-n`* *`Betted_Numbers`* *`-i`* *`Estratto_Betted_Money`* *`Estratto_Winned_Money`* *`Ambo_Betted_Money`* *`Ambo_Winned_Money`* *`Terno_Betted_Money`* *`Terno_Winned_Money`* *`Quanterna_Betted_Money`* *`Quaterna_Winned_Money`* *`Cinquina_Betted_Money`* *`Cinquina_Winned_Money`* <br>
where:<br>
*Betted_Wheels_Res* is a bool that rapresent if the client won on that wheel or not;
*Betted_Wheels* could rapresent more than one wheel;
*Betted_Numbers* could rapresent more than one number;
*???_Betted_Money* rapresent how much money the client betted on;
*???_Winned_Money* rapresent how much money the client winned with that combination;<br>
Eg. <br>
`1610982560 -r 0 Roma -n 45 15 -i 0 0.00 5 0.00 0 0.00 0 0.00 0 0.00`<br>
`1610982560 -r 1 Tutte -n 15 12 13 25 -i 45 34.46 20 0.00 0 0.00 0 0.00 0 0.00`<br>
`1610982560 -r 0 Napoli -n 88 -i 65 0.00 0 0.00 0 0.00 0 0.00 0 0.00 `

#### /txt/logs.txt
The logs file's format is: *`username`* *`client IP`* *`attempts number`* *`timestamp`*<br>
where:<br>
*attempts number* is a number (0,3) that rapresent if the client putted the correct credentials.<br>
Eg.<br>
`pippo 192.168.41.12 1 1610982286`<br>
`pippo 192.168.41.12 0 1610983928`<br>
`pluto 192.168.25.65 0 1610983968`

#### /txt/blacklist.txt
The blacklist file's format is: *`client IP`* *`timestamp`*<br>
Eg.<br>
`192.168.41.12 1610984661`

#### /txt/actual_plays.txt
The actual_plays file's format is: *`username`* *`-r`* *`Betted_Wheels`* *`-n`* *`Betted_Numbers`* *`-i`* *`Betted_Money`*<br>
Eg.<br>
`pippo -r Roma -n 45 15 -i 0 5`<br>
`pippo -r Tutte -n 15 12 13 25 -i 45 20`

#### /txt/actual_extraction.txt
The actual_extraction file's format is: *`Wheel`* *`Number 1`* *`Number 2`* *`Number 3`* *`Number 4`* *`Number 5`* <br>
Eg.<br>
`Bari 61 79 63 50 28`<br>
`Cagliari 82 66 70 38 10`<br>
`Firenze 74 26 77 55 57`<br>
`Genova 79 75 61 35 15`<br>
`Milano 52 36 12 30 71`<br>
`Napoli 65 61 21 46 5`<br>
`Palermo 48 69 45 20 28`<br>
`Roma 73 63 56 14 11`<br>
`Torino 27 50 88 13 66`<br>
`Venezia 16 53 50 76 87`<br>
`Nazionale 27 37 32 38 28`<br>

#### /txt/past_extraction.txt
The past_extraction file's format is: *`Wheel`* *`Number 1`* *`Number 2`* *`Number 3`* *`Number 4`* *`Number 5`* <br>
Eg.<br>
`Bari 61 79 63 50 28`<br>
`Cagliari 82 66 70 38 10`<br>
`Firenze 74 26 77 55 57`<br>
`Genova 79 75 61 35 15`<br>
`Milano 52 36 12 30 71`<br>
`Napoli 65 61 21 46 5`<br>
`Palermo 48 69 45 20 28`<br>
`Roma 73 63 56 14 11`<br>
`Torino 27 50 88 13 66`<br>
`Venezia 16 53 50 76 87`<br>
`Nazionale 27 37 32 38 28`<br>
`Bari 6 45 65 75 28`<br>
`Cagliari 35 12 74 1 5`<br>
`...`<br>






