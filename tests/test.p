program example(input,output);
    var x,y:integer; arr:array [2..6] of char;
    function gcd(a,b:integer):integer;
        begin 
            if b=0 then gcd:=a { this is a comment }
            else gcd:=gcd(b, a mod b)
        end;
    begin
        read(x, y, arr[5]);
        write(gcd(x, y), arr[6]);
        if (a = 1) then
            j:=1
        else
            j:=0;
        for i:=a=10 to a=10 do 
            a:=10
    end.
