# IIS FIT VUT
project of IIS subject @ FIT BUT, Brno

it takes a lot ~

---
Variant: 3. Hromadna doprava: zadavani, vyhledavani spoju a rezervaci jizdenek


### How to use mysql DB
db is run on google cloud MySQL
has 100GB space, 4CPUs, 8 Gbs of RAM (I think)
1) use mysql-workbench
  - create new connection
    host = 34.118.28.197
    user = (not implemented yet) -- probably use something like "dev" for management
                                 -- and "user" for viewing or something
    password = ask admin :pepelaugh:
    port = 3306
    (should be all)

2) use mysql cml directly
  - mysql -h host -P port -u user -p
    (something like this) should be the syntax
    troubleshooting:
      * (Archlinux): dependency: mysql(mariadb) -- running process?
      * needs to have client IP (you) in DB config on google cloud (for safety)
      -- 'curl ifconfig.cz' / 'google search: my ip' etc. -> add to GCDB(google cloud database)
