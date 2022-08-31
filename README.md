# borg-rsh-local
Emulate ssh connection for local borg repository.

__Borgbackup__ <a href="https://borgbackup.readthedocs.io/">https://borgbackup.readthedocs.io/</a> can use a remote shell (*ssh*) connection from the dumping client to the server containing the repository.
````
clntuser@client $  borg create -x dumpuser@repohost:/borg-repo::example /example
````
Here the user _clntuser_ is creating a repo on _repohost_ as user _dumpuser_ by
````
ssh dumpuser@repohost borg serve --umask 077
````
The actual ssh command can be specified on the borg command line (_--rsh_) or via the _BORG_RSH_ environment variable.

Where the client and repository are on the same machine but are different users - typically __root__ (to access all files)
and __dumpuser__ (for consistency with remote backups or where the repository is an __NFS__ _mounted volume_ and __root_squash__ is in effect) one
suggestion is to use _ssh_ to localhost
````
dumpuser@localhost:/borg-repo
````
which has the overhead of creating clients _ssh keys_ and configuring _~dumpuser/.ssh/authorized_keys_ which for a root client
is redundant. In use there are an added _ssh_ and a _sshd_ process between the _borg client_ and _borg backend_.

__borg-rsh-local__ emulates enough of a ssh connection between the borg client (running as _root_) and the borg backend running as _dumpuser_
but unlike _ssh-sshd_ removes itself by a __execve(2)__ of the borg backend.

Typical use with __/sbin/opt/borg-rsh-local__
````
root@repohost# borg init --rsh "/sbin/opt/borg-rsh-local -r /borg" -e none dumpuser@localhost:/borg/borg-repo

# With BORG_RSH
root@repohost# export BORG_RSH="/sbin/opt/borg-rsh-local -r /borg"
root@repohost# borg create -x dumpuser@repohost:/borg-repo::example /example
````

## NOTES
_borg-rsh-local_ __refuses__ to _execve(2)_ the borg backend as root.

The _-r /directory_ option of _borg-rsh-local_ is passed to the _---restrict-to-path_ option of the _borg serve_ backend.

_borg-rsh-local_ constructs _SSH_ORIGINAL_COMMAND_ variable emulating ssh. 

## BUGS
Doubtless legion.

## SEE ALSO
<a href="https://borgbackup.readthedocs.io/">https://borgbackup.readthedocs.io/</a>

borgbackup(1) <a href="https://manpages.ubuntu.com/manpages/impish/en/man1/borgbackup.1.html">
https://manpages.ubuntu.com/manpages/impish/en/man1/borgbackup.1.html</a>
