
# include	<unistd.h>
# include	<errno.h>
# include	<stdlib.h>
# include	<pwd.h>

# include	"errors/errors.h"
# include	"memory/memory.h"
# include	"string/str.h"
# include	"string/strvec.h"
# include	"system/passwd.h"

static	result_t	str_passwd_make (str_passwd_t* pw, struct passwd* pwd){
	result_t	result	= err;
		
	str_t*	pw_name		= 0;
	str_t*	pw_passwd	= 0;
	str_t*	pw_gecos	= 0;
	str_t*	pw_dir		= 0;
	str_t*	pw_shell	= 0;
		
	result	= str_Create_cstring (&pw_name, pwd->pw_name);
	if (result==ok)
		result	= str_Create_cstring (&pw_passwd, pwd->pw_passwd);
	if (result==ok)
		result	= str_Create_cstring (&pw_gecos, pwd->pw_gecos);
	if (result==ok)
		result	= str_Create_cstring (&pw_dir, pwd->pw_dir);
	if (result==ok)
		result	= str_Create_cstring (&pw_shell, pwd->pw_shell);
	if (result==ok){
		*pw	= (str_passwd_t){
			.pw_name	= pw_name,
			.pw_passwd	= pw_passwd,
			.pw_uid		= pwd->pw_uid,
			.pw_gid		= pwd->pw_gid,
			.pw_gecos	= pw_gecos,
			.pw_dir		= pw_dir,
			.pw_shell	= pw_shell,
		};
	}
	else	{ // _Delete (p) where p==0 or *p==0 is nullop
		str_Delete (&pw_name);
		str_Delete (&pw_passwd);
		str_Delete (&pw_gecos);
		str_Delete (&pw_dir);
		str_Delete (&pw_shell);
	}
	return	result;
}
result_t	str_passwd_Create (str_passwd_t** pwp, struct passwd* cpwd){
	str_passwd_t*	pw	= 0;
	result_t	result	= mem_allocate (&pw, sizeof (*pw));
	if (result==ok && cpwd) {
		result	= str_passwd_make (pw, cpwd);
		if (result != ok)
			free (pw);
	}
	if (result==ok)
		*pwp	= pw;
	return	result;
}
result_t	str_passwd_Delete (str_passwd_t** pwp){
	result_t	result	= ERRORS_STD_EINVAL;
	if (pwp) {
		str_passwd_t*	pw	= *pwp;
		if (pw) {
			str_Delete (&pw->pw_name);
			str_Delete (&pw->pw_passwd);
			str_Delete (&pw->pw_gecos);
			str_Delete (&pw->pw_dir);
			str_Delete (&pw->pw_shell);
			result	= mem_free (&pw);
			*pwp	= 0;
		}
		result	= ok;
	}
	return	result;
}
		
result_t	str_getpwnam (str_passwd_t* pw, str_t* user) {
	result_t	result	= ERRORS_STD_EINVAL;
	if (user!=0 && pw!=0) {
		char*	name	= str_cstring (user);
		struct	passwd*	pwd	= getpwnam (name);
		if (pwd) {
			result	= str_passwd_make (pw, pwd);
		}
		else	result	= ERRORS_STD_ERRNO (errno);
	}
	return	result;
}
result_t	str_getpwnam_r (str_passwd_t** pwp, str_t* user) {
	str_passwd_t*	pw	= 0;
	result_t	result	= str_passwd_Create (&pw, 0);
	if (result==ok) {
		result	= str_getpwnam (pw, user);
		if (result==ok) 
			*pwp	= pw;
		else
			str_passwd_Delete (&pw);
	}
	return	result;
}
result_t	str_getpwuid (str_passwd_t* pw, uid_t uid) {
	result_t	result	= ERRORS_STD_EINVAL;
	if (pw){
		struct	passwd*	pwd	= getpwuid (uid);
		if (pwd) {
			result	= str_passwd_make (pw, pwd);
		}
		else	result	= ERRORS_STD_ERRNO (errno);
	}
	return	result;
}
result_t	str_getpwuid_r (str_passwd_t** pwp, uid_t uid) {
	str_passwd_t*	pw	= 0;
	result_t	result	= str_passwd_Create (&pw, 0);
	if (result==ok) {
		result	= str_getpwuid (pw, uid);
		if (result==ok) 
			*pwp	= pw;
		else
			str_passwd_Delete (&pw);
	}
	return	result;
}
