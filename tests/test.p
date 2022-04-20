program example(input,output);
    var x,y:integer; arr:array [2..6] of char;
    function gcd(var a:integer; b:integer):integer;
        var foo:array [3..9] of integer;
        begin 
            if b=0 then gcd:=a { this is a comment } else gcd:=gcd(b, a mod b);

            gcd(b+b, foo[5]);
            gcd(foo[5], foo[5]);

            a:=4;

            read(a, b); 
            write(a, b, a+b);

            if a = 3 then a:=4 else exit;
            if gcd(a, a) then a:=4 else gcd(a, b + 1);
        end;


    begin
        read(x, y, arr[5]);
        write(gcd(x, y), arr[6] + arr[7]);

        if (x = 1) then;

        for i:=x=10 to x=10 do;

        if gcd(arr[3] + arr[4], arr[4]) then;
        gcd(arr[3], arr[4]);
    end.
