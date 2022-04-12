program example(input,output);
    var x,y:integer; 
        z:char; 
        arr:array[5..10, 3..6] of integer;

    function gcd(a,b:integer):integer;
        begin 
            if b=0 then gcd:=a { this is a comment }
            else gcd:=gcd(b, a mod b)
        end;

    begin
        read(x, y, z);
        write(x, gcd(y, arr[5, 5]));    
    end.
