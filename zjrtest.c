void testtable()	//print table //zjr 11.17 //test code
{
	mask *mk;
	array *arr;
	dimensionHead *dhead;
	dimension *dim;
	int flag=0;
	int i;
	//tmppram mode
	/*for (i=0;i<funcparam;++i)	//tx for symbol table   funcparam for tmpparam
	{
		if (tmpparam[i].kind==ID_VARIABLE)
		{
			flag=0;
			mk=(mask *)&tmpparam[i];
			printf("%d :variable %s\n",i,mk->name);
		}
		else if (tmpparam[i].kind==ID_ARRAY)
		{
			if (flag==0)	//head
			{
				arr=(array *)&tmpparam[i];
				printf("%d :array %s level:%d\n",i,arr->name,arr->level);
				flag=1;
			}
			else if (flag==1)	//dim head
			{
				dhead=(dimensionHead *)&tmpparam[i];
				printf("%d :array %s depth:%d\n",i,dhead->name,dhead->depth);
				flag=2;
			}
			else
			{
				dim=(dimension *)&tmpparam[i];
				printf("%d :array %s width:%d\n",i,dim->name,dim->width);
			}
		}
	}
*/
	//symbol table mode
	for (i=0;i<=tx;++i)	//tx for symbol table   funcparam for tmpparam
	{
		if (table[i].kind==ID_VARIABLE)
		{
			flag=0;
			mk=(mask *)&table[i];
			printf("%d :variable %s\n",i,mk->name);
		}
		else if (table[i].kind==ID_ARRAY)
		{
			if (flag==0)	//head
			{
				arr=(array *)&table[i];
				printf("%d :array %s level:%d\n",i,arr->name,arr->level);
				flag=1;
			}
			else if (flag==1)	//dim head
			{
				dhead=(dimensionHead *)&table[i];
				printf("%d :array %s depth:%d\n",i,dhead->name,dhead->depth);
				flag=2;
			}
			else
			{
				dim=(dimension *)&table[i];
				printf("%d :array %s width:%d\n",i,dim->name,dim->width);
			}
		}
		else if (table[i].kind==ID_PARRAY)
		{
			printf("%d :parray %s \n",i,table[i].name);
			flag=0;
		}
		else
		{
			mk=(mask *)&table[i];
			printf("%d :%d %s level:%d addr:%d\n",i,mk->kind,mk->name,mk->level,mk->address);
			flag=0;
		}
	}//table mode
	
	printf("dx is %d ;tx is %d\n",dx,tx);
	//exit(2);
}

void mytest()	//for test //zjr 11.19
{
	printf("here\n");//tc
}
