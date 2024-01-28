# Simple Http Server

A HTTP/1.1 Server implemented in C.

For testing in this state of the project, the /etc/hosts need to change. The following lines need to be added:

```
    127.0.0.1	www.homem_macaco.com
    127.0.1.1	www.doutor_casa.com
    127.0.1.1	doutor_casa.com
```


# TODO 


+ ORGANIZE THE PROJECT
+ REFACTOR MESSAGE PARSING (3)
+ REFACTOR START LINE PARSING (3)
+ REFACTOR LOGIC OF STATUS LINE (3.1)
+ IMPLEMENT HEADER FIELDS PARSING (3.2)
+ NEW REQUEST HEADER FIELDS (5)
+ REVIEW STATUS CODES/ MESSAGES (6.1)
+ REVIEW FIELD VALUE COMPONENT IN RFC (3.2.6)
+ REFACTOR REQUEST TARGET PARSING (5.3)
+ CHANGE PERSISTENCE CONDITIONS (6.3)
    + SEE ALSO (6.3.2 & 6.3.3)