int fibonacci(int n){
    int c; 
    int fn; 
    int fmenos;	
    c = 1;	
    fn = 1;	
    fmenos = 0;	
    while(c<n){
        int aux;		
        aux = fmenos;	
        fmenos = fn;		
        fn = fn + aux;	
        c = c + 1;	
    }	
    return fn;
}

void main(void){	
    output(fibonacci(5));
}
