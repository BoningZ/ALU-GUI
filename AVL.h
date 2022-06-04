#include<cstdio>
#include<algorithm>
#include<string>
#include<QJsonObject>
#include<QJsonArray>
struct AVLnode{
	int dat,h,cnt,siz;//ֵ���ߣ���ֵ������������С
	AVLnode* son[2];//��0��1 
	AVLnode(int x):dat(x),h(1),cnt(1),siz(1){son[0]=son[1]=NULL;}
};
inline int get_h(AVLnode* p){return p==NULL?0:p->h;}
inline int get_siz(AVLnode* p){return p==NULL?0:p->siz;}
struct AVL{
	AVLnode* root;
	inline void update(AVLnode*& o){//���½ڵ� 
		o->siz=get_siz(o->son[0])+get_siz(o->son[1])+o->cnt;
		o->h=get_h(o->son[get_h(o->son[1])>get_h(o->son[0])])+1;
	}
	inline void single_rotate(AVLnode*& o,int op){//��������son[op]�ڵ�ת��o 
		AVLnode* p=o->son[op];
		o->son[op]=p->son[op^1];
		p->son[op^1]=o;
		update(o);update(p);
		o=p;
	}
	inline void double_rotate(AVLnode*& o,int op){//˫�� 
		single_rotate(o->son[op],op^1);
		single_rotate(o,op);
	}
	void insert(AVLnode*& o,int x){//���� 
		if(!o){o=new AVLnode(x);return;}
		if(o->dat==x){(o->cnt)++;update(o);return;}//�Ѵ��ڣ�ֱ�Ӽ�¼+1 
		int op=(x>o->dat);//�������� 
		insert(o->son[op],x); update(o);
		if(get_h(o->son[op])-get_h(o->son[op^1])==2)
			if(op^(x>o->son[op]->dat))double_rotate(o,op);//��������ͬ��˫�� 
			else single_rotate(o,op);//����������ͬ������ 
		update(o);
	}
	void erase(AVLnode*& o,int x){//ɾ�� 
		if(!o)throw -1;
		if(o->dat==x){//���ҵ�Ҫɾ���ĵ� 
			if(o->cnt>1){(o->cnt)--;update(o);return;}//��ֵ����һ����ֱ�Ӽ�¼-1 
			if(o->son[0]&&o->son[1]){//�������������� 
				AVLnode* p=o->son[1];
				while(p->son[0])p=p->son[0];
				o->cnt=p->cnt; o->dat=p->dat;
				p->cnt=1; erase(o->son[1],p->dat);
				update(o);
				if(get_h(o->son[0])-get_h(o->son[1])==2)//��������ɾ�㣬����ΪRx��ɾ�� 
					if(get_h(o->son[0]->son[0])>=get_h(o->son[0]->son[1]))single_rotate(o,0);
					else double_rotate(o,0);
			}else{
				AVLnode* p=o;
				if(!(o->son[0])&&!(o->son[1]))o=NULL;//��������ֱ��ɾ 
				else o=o->son[o->son[1]!=NULL];//����ֻ��һ��������ֱ�������� 
				delete p;
			}
		}else{//�������� 
			int op=(x>o->dat);
			erase(o->son[op],x);update(o);
			if(get_h(o->son[op^1])-get_h(o->son[op])>=2)
				if(get_h(o->son[op^1]->son[op^1])>=get_h(o->son[op^1]->son[op]))
					single_rotate(o,op^1);//ͬһ���������أ����� 
				else double_rotate(o,op^1);//��ͬ�����أ�˫�� 
		}
		if(o==NULL)return;
		update(o);	
	}
	int idx2dat(AVLnode* o,int idx){//��o���������в�������Ϊidx�Ľڵ�dat 
		if(o==NULL)throw -1;
		if(get_siz(o->son[0])>=idx)return idx2dat(o->son[0],idx);//���� 
		if(get_siz(o->son[0])+o->cnt>=idx)return o->dat;
		return idx2dat(o->son[1],idx-get_siz(o->son[0])-o->cnt);//���� 
	}
	int dat2idx(AVLnode* o,int dat){//��o���������в���dat������ 
		if(o==NULL)throw -1;
		if(o->dat==dat)return get_siz(o->son[0])+1;
		if(o->dat>dat)return dat2idx(o->son[0],dat);//���� 
		return dat2idx(o->son[1],dat)+get_siz(o->son[0])+o->cnt;//���� 
	}
    std::string ascend(AVLnode* o){//�������
        if(o==NULL)return "";
        std::string st;
        for(int i=0;i<o->cnt;i++)st+=std::to_string(o->dat)+" ";
        st+=ascend(o->son[0]); st+=ascend(o->son[1]);
        return st;
	}
	int build(AVLnode*& o,int a[],int b[],int siz){//a����b���� 
		o=new AVLnode(a[0]);
		if(siz==1)return 1;
		int lch=std::lower_bound(b,b+siz,a[0])-b;//��ȡ 
		int res=1;
		if(lch>0)res&=build(o->son[0],a+1,b,lch);
		if(siz-lch-1>0)res&=build(o->son[1],a+1+lch,b+1+lch,siz-lch-1);
		update(o);
		int balance=get_h(o->son[0])-get_h(o->son[1]);
		if(balance>=2||balance<=-2)res=false;
		return res;
	}
    void fillJson(QJsonArray& data,QJsonArray& links,AVLnode* o,int l,int r,int y){
        QJsonObject nodeInfo;
        nodeInfo.insert("name",o->dat);
        nodeInfo.insert("x",(l+r)>>1);
        nodeInfo.insert("y",y);
        nodeInfo.insert("balance",get_h(o->son[0])-get_h(o->son[1]));
        data.push_back(nodeInfo);

        for(int op=0;op<=1;op++)
            if(o->son[op]){
                QJsonObject linkInfo;
                linkInfo.insert("source",o->dat);
                linkInfo.insert("target",o->son[op]->dat);
                links.push_back(linkInfo);
            }
    }
    inline QJsonObject toJson(){
        QJsonObject res;
        QJsonArray data,links;
        fillJson(data,links,root,0,750,75);
        res.insert("data",data);
        res.insert("links",links);
    }
	inline bool Build(int a[],int siz){
		int *b=new int[siz];
		for(int i=0;i<siz;i++)b[i]=a[i];
		std::sort(b,b+siz);//������� 
		if(std::unique(b,b+siz)!=b+siz)return 0;//�������ظ� 
		return build(root,a,b,siz);
	}
	inline void Insert(int x){insert(root,x);}
	inline int Search(int dat){return dat2idx(root,dat);}
	inline void Delete(int x){erase(root,x);}
	inline int IndexSearch(int idx){return idx2dat(root,idx);}
	inline void IndexDelete(int idx){erase(root,idx2dat(root,idx));}
    inline std::string Ascend(){ascend(root);}

};
AVL avl;
int main(){
}
	
			 
	
		
