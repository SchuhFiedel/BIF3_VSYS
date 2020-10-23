# BIF3_VSYS
##VSYS Project

###Client Server Communication
. Es werden Chars bzw Strings zwischen client und server übertragen, diese werden dann je nach Situation anders Verwendet und Ausgewertet
. Mails können nur an Bestehende user (also bereits erstellte folder im Mailfolder) geschickt werden
. Für jeden user welcher sich angemeldet hat wird ein neuer Folder mit dessen namen erstellt in welchen die nachrichten welche dieser empfängt gespeichert werden
..

###Server Vorraussetzungen / Start
. ./myserver.o PortNumber MailFolder
. Mailfolder muss nicht vor der Ausführung des Programms angelegt werden
..

###Client Vorraussetzungen / Start
. ./myclient.o ServerAdress PortNumber Username(FHUID)
. after the user has started the program they are prompted to input their password
. if they enter it wrong 4 times they are blocked from doing so for 30 seconds
..
