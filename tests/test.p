program ref(input,output);
    var x,y: integer;
    function writes(var a:integer; b:integer):integer;
        begin 
            a:=4;
            read(a, b); 
            write(a, b);
            if a = 3 then a:=4 else writes(a, a);
            if writes(a, a) then a:=4 else writes(a, b + 1);
        end;

    begin
        writes(x, y);
        writes(y, x+1);
    end.
