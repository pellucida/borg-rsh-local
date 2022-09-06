/*
// @(#)	passwd.h
*/
# if	!defined(SYSTEM_PASSWD_H)
# define	SYSTEM_PASSWD_H	1

# include	<unistd.h>
# include	<pwd.h>

struct	str_passwd_t	{
	str_t*	pw_name;
	str_t*	pw_passwd;
	uid_t	pw_uid;
	gid_t	pw_gid;
	str_t*	pw_gecos;
	str_t*	pw_dir;
	str_t*	pw_shell;
};
typedef	struct	str_passwd_t	str_passwd_t;

result_t	str_passwd_Create (str_passwd_t** pwp, struct passwd* cpwd);
result_t	str_passwd_Delete (str_passwd_t** pwp);
result_t	str_getpwnam (str_passwd_t* pw, str_t* user);
result_t	str_getpwnam_r (str_passwd_t** pwp, str_t* user);
result_t	str_getpwuid (str_passwd_t* pw, uid_t uid);
result_t	str_getpwuid_r (str_passwd_t** pwp, uid_t uid);

# endif
