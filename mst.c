#include<iostream.h>
#include<stdlib.h>
#include<stdio.h>

int p[10];

void kruskal(int w[10][10],int n)
{
    int min,sum=0,ne=0,i,j,u,v,a,b;
    for(i=1;i<=n;i++)
    p[i]=0;
    while(ne<n-1)
    {
        min=999;
        for(i=1;i<=n;i++)
        for(j=1;j<=n;j++)
        {
            if(w[i][j]<min)
            {
                min=w[i][j];
                u=a=i;
                v=b=j;
            }
        }
        while(p[u])
            u=p[u];
        while(p[v])
            v=p[v];
        if(u!=v)
        {
            ne++;
            sum+=min;
            cout<<"\nedge "<<a<<"-->"<<b<<" is "<<min;
            p[v]=u;
        }
        w[a][b]=w[b][a]=999;
    }
    cout<<"\nmin cost spanning tree= "<<sum;
}

void main()
{
    int w[10][10],n,i,j;
    clrscr();
    cout<<"enter no.of vertices\n";
    cin>>n;
    cout<<"enter weight matrix\n";
    for(i=1;i<=n;i++)
        for(j=1;j<=n;j++)
            cin>>w[i][j];
    for(i=1;i<=n;i++)
        for(j=1;j<=n;j++)
            if(w[i][j]==0)
                w[i][j]=999;
    kruskal(w,n);
}
