require "k"
c=k.khp()
k.ks(c,"a:4+2")
k.ks(c,"{b::x}", 69)
k.ks(c,"{c::x}", 69.42)
k.ks(c,"{d::x}", "fishcakes")
k.ks(c,"{e::x}", nil)
k.ks(c,"{f::x}", {3,6,9,12})
k.ks(c,"{g::x}", {true,true,false,true})
k.ks(c,"{h::x}", {"fish",42,"cat"});
k.ks(c,"{i::x}", true);
k.ks(c,"{j::x}", {foo="bar",cow="mom"})
print(k.k(c,"\"test\""));
print(k.k(c,"T"));


