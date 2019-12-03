#include <lauxlib.h>

#include "kx/k.h"

#include <pthread.h>
#include <string.h>
#include <math.h>

#define RI(i) {lua_pushinteger(L,i);R 1;}
#define LI(x) ((int)(floor(x)-1))
static K __thread conn_open = 0;

static int enc(K*k,lua_State *L)
{
	switch (lua_type(L, -1)) {
	case LUA_TSTRING: 	{ size_t len;const char *str = lua_tolstring(L,-1,&len);(*k)=kpn(str,len);R 1;}	break;
	case LUA_TNUMBER:	{ F num = lua_tonumber(L,-1);(*k) = (num==floor(num))?kj((J)num):kf(num);R 1;} break;
	case LUA_TBOOLEAN:	{ (*k)=kb( lua_toboolean(L,-1) );R 1;}	break;
	case LUA_TNIL:		{ (*k)=ktn(0,0);R 1;}	break;
	case LUA_TTABLE:	{
		double p;
		int max = 0;
		int items = 0;
		int t_integer = 0, t_number = 0, t_boolean = 0, t_other= 0;

		lua_pushnil(L);
		/* table, startkey */
		while (lua_next(L, -2) != 0) {
			items++;

			/* table, key, value */
			switch (lua_type(L, -1)) {
			case LUA_TNUMBER:  t_number++; p = lua_tonumber(L,-1);  t_integer += (floor(p) == p); break;
			case LUA_TBOOLEAN: t_boolean++; break;
			default:   t_other++; break;	/* or anything else */
			};

			if (lua_type(L, -2) == LUA_TNUMBER &&
					(p = lua_tonumber(L, -2))) {
				/* Integer >= 1 ? */
				if (floor(p) == p && p >= 1) {
					if (p > max)
						max = p;
					lua_pop(L, 1);
					continue;
				}
			}

			/* Must not be an array (non integer key) */
			for (lua_pop(L,1); lua_next(L, -2) != 0; lua_pop(L,1)) ++items;
			max = 0;
			break;
		}
		lua_pushnil(L);
		if (max != items) {
			/* build K dictionary */
			K keys = ktn(KS,items);
			K values = ktn(0,items);
			int n = 0;
			/* table, startkey */
			while (lua_next(L, -2) != 0) {
				kS(keys)[n] = ss(lua_tostring(L, -2));
				if(!enc(kK(values)+n,L))R 0;
				lua_pop(L,1);
				++n;
			}
			*k = xD(keys,values);
			R 1;
		}
		/* build K list */
		if(t_other || ((!!t_boolean)+(!!t_number)) > 1) {
			K a = ktn(0,items);
			while (lua_next(L, -2) != 0) {
				p = lua_tonumber(L, -2);
				if(!enc(kK(a)+LI(p),L))R 0;
				lua_pop(L, 1);
			}
			*k = a;
			R 1;
		}
		if(t_boolean) {
			K a = ktn(KB,items);
			while (lua_next(L, -2) != 0) {
				p = lua_tonumber(L, -2);
				kG(a)[LI(p)] = lua_toboolean(L,-1);
				lua_pop(L, 1);
			}
			*k = a;
			R 1;
		}
		if(t_number == t_integer) {
			K a = ktn(KJ,items);
			while (lua_next(L, -2) != 0) {
				p = lua_tonumber(L, -2);
				kJ(a)[LI(p)] = (int)floor(lua_tonumber(L,-1));
				lua_pop(L, 1);
			}
			*k = a;
			R 1;
		}
		if(t_number) {
			K a = ktn(KF,items);
			while (lua_next(L, -2) != 0) {
				p = lua_tonumber(L, -2);
				kF(a)[LI(p)] = lua_tonumber(L,-1);
				lua_pop(L, 1);
			}
			*k = a;
			R 1;
		}
		*k = ktn(0,0);
		R 1;
	}; break;
	default:
		luaL_error(L, "Cannot serialise %s: %s", lua_typename(L, lua_type(L, -1)), "can't serialize type");
		R 0;
	};
}
#define LD(a,b) lua_createtable(L,xn,0);DO(xn,b(L,a(x)[i]);lua_rawseti(L,-2,i));
static int dec(lua_State* L,K x)
{
	if(xt >= 0) {
		switch(xt) {
		case 0: LD(kK,dec); break;
		case 1: LD(kG,lua_pushboolean);break;
		//case 2: /* guid */
		case 4: LD(kG,lua_pushinteger);break;
		case 5: LD(kH,lua_pushinteger);break;
		case 6: LD(kI,lua_pushinteger);break;
		case 7: LD(kJ,lua_pushnumber);break;
		case 8: LD(kE,lua_pushinteger);break;
		case 9: LD(kF,lua_pushinteger);break;
		case 10:lua_pushlstring(L,kG(x),xn);break;
		case 11:LD(kS,lua_pushstring);break;
		//case 12: /* timestamp */
		//case 13: /* month */
		//case 14: /* date */
		//case 15: /* datetime */
		//case 16: /* timespan */
		//case 17: /* minute */
		//case 18: /* second */
		//case 19: /* time */
		//case 98: /* dict */
		//case 99: /* table */

		default:luaL_error(L, "unsupported array %d (nyi?)", xt);R 0;
		};
		R 1; /* create table */
	}
	switch(xt) {
	case -1: lua_pushboolean(L,x->g); break;
	//case -2: /* scalar guid */
	case -4: lua_pushinteger(L,x->g); break;
	case -5: lua_pushinteger(L,x->h); break;
	case -6: lua_pushinteger(L,x->i); break;
	case -7: lua_pushnumber(L,x->j); break;
	case -8: lua_pushnumber(L,x->e); break;
	case -9: lua_pushnumber(L,x->f); break;
	case -10: lua_pushlstring(L,&x->g,1); break;
	case -11: lua_pushstring(L,x->s); break;
	case -128:luaL_error(L,"K: %s",kS(x));R 0;
	default:luaL_error(L, "unsupported scalar %d (nyi?)", xt);R 0;
	};
	R 1;
}

static void clean(void*_)
{
	K x = conn_open;
	if(x) {
		DO(xn, kclose(xI[i]));
		r0(x);
	}
	conn_open = 0;
	m9();
}
static int conn(int fd)
{
	if(!conn_open) {
		pthread_key_t rr;
		pthread_key_create(&rr,clean);
		conn_open = ktn(6,0);
	}
	conn_open = ja(&conn_open, &fd);
	return fd;
}

static int wrap_khp(lua_State* L)
{
	S h=luaL_optstring(L,1,"0");
	I p=luaL_optint(L,2,5000);
	S u=luaL_optstring(L,3,0);
	I t=luaL_optint(L,4,-1);
	if(t<0){if(!u)RI(conn(khp(h,p)));RI(conn(khpu(h,p,u)));}
	RI(conn(khpun(h,p,u,t)));
}
static int wrap_kclose(lua_State*L)
{
	int c, n = lua_gettop(L);
	if(!n) {
		K x = conn_open;
		if(x) {
			DO(xn, kclose(xI[i]));
			xn = 0;
		}
	} else {
		c=luaL_optint(L,1,-1);
		if(c < 0) RI(0);
		K x=ktn(6,conn_open->n); xn=0;
		DO(conn_open->n,(x=((n=kI(conn_open)[i])==c)?kclose(c),c=-1,x:ja(&x,&n)));
		r0(conn_open);
		conn_open = x;
		if(c != -1) RI(0);
	}
	RI(1);
}
static int dok(int f,lua_State*L)
{
	int c, n = lua_gettop(L);

	/* k [int] "kcode" [x [y [z]]] */
	if (n>0 && lua_type(L, 1) == LUA_TNUMBER) {
		c = (int)lua_tonumber(L,1);
		--n;
	} else {
		if(!conn_open || !conn_open->n) RI(0);
		c = kI(conn_open)[conn_open->n - 1];
	}
	/* k "kcode" [x [y [z]]] */
	if(!n) {luaL_argcheck(L, 0, 1, "expected 1 argument"); R -1;}

	K x=0,y=0,z=0;
	if(n>1) {         enc(&x,L);lua_pop(L,1);--n; }
	if(n>1) {     y=x;enc(&x,L);lua_pop(L,1);--n; }
	if(n>1) { z=y;y=x;enc(&x,L);lua_pop(L,1);--n; }
	if(n>1) { luaL_argcheck(L,0,lua_gettop(L), "too many arguments (nyi?)"); r0(x);r0(y);r0(z); R -1; }
	const char *code = lua_tostring(L,-1);
	K r=k(c*f,code,x,y,z,(K)0);
	if(f==-1){/*async*/ lua_pushnil(L); R 1; }
	dec(L,r);r0(r); R 1;
}
static int wrap_k(lua_State*L)
{
	R dok(1,L);
}
static int wrap_ks(lua_State*L)
{
	R dok(-1,L);
}

int luaopen_k (lua_State *L)
{
	static int did_open = 0;
	static const struct luaL_Reg _k [] = {
		{"khpun",wrap_khp}, {"khpu",wrap_khp}, {"khp",wrap_khp},
		{"kclose",wrap_kclose},
		{"k",wrap_k}, {"ks",wrap_ks},
		{NULL,NULL}
	};
	if(!did_open) {
		khp("",-1);
		setm(did_open = 1);
	}
	
	luaL_register(L, "k", _k);
	return 1;
}

