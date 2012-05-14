    #include <iostream>  
    using namespace std;  
      
    int getV(char p, const char* set)  
    {  
        for(int i=0; i<strlen(set); i++)  
        {  
            if (p==set[i])  
                return i;  
        }  
        return -1;  
    }  
      
    int RK(const char* T, const char* P,const char* set)  
    {  
        int base = strlen(set) ;  
        int n = strlen(T);  
        int m = strlen(P);  
        int h = 1;  
        for(int i=0;i<m-1;i++)  
            h*=base;  
        int p = 0;  
        int t = 0;  
        for(int i=0; i<m; i++)  
        {  
            p = base*p + getV(P[i],set);  
            t = base*t + getV(T[i],set);  
        }  
        for (int i=0; i<=n-m; i++)  
        {  
            cout<<"p,t is "<<p<<","<<t<<endl;  
            if (p==t) return i;  
            if (i<n-m)  
                t = getV(T[i+m],set)+base*( t - h*getV(T[i],set) );  
        }  
        return -1;  
    }  
      
    int main()  
    {  
        // set is the character set  
        const char* set= "0123456789";  
        const char* T = "258569236589780";  
        const char* P = "2365";  
        int i = RK(T, P, set);  
        cout<<"the postition is:"<<i<<endl;  
        return 0;  
    }  