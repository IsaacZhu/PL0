var result,a,i,j;
begin
        j:=0;
        i:=0;
	result:=5;
        for(;i<10;i++)
        {
            if(i==5)
            then
            {
                    for(a:=0;a<i;a++)
                    {
                            if(a==3)
                            then
                            {
                                    result:=2;
                                    break;
                            };
                            else ;
                    };
            };
            else result:=1;
            if(i==9)
            then continue;
            else ;
            j++;
        };
        printf("hello @0 @1 @2\n", a,i,result);
end.