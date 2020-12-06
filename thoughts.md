# To consider
Do I need to implement my own history? or can I realy on the system's
Recieving from a sender should be handles with interups or polling

# findings and limiatations
Will not work if functions or environment variables have been set that affect the ability for parent procesess to manipulate and interact with their child processes. A prime example is madvice() command and its MADV_DONTFORK flag.




# Manuals to reference
core
madvice
ptrace
fork
