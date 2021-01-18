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
