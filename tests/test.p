program example(input,output);
    var x,y:integer;
    function gcd(a,b:integer):integer;
        begin 
            if b=0 then gcd:=a { this is a comment }
            else gcd:=gcd(b, a mod b)
        end;
    begin
        read(x, y);
        write(gcd(x, y))
    end.
