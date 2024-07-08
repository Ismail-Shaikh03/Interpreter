#include "val.h"
using namespace std;


Value Value::operator*(const Value& op)const{
    
    if(this->IsReal() || this->IsInt()){
        if(op.T != VREAL && op.T !=VINT){
            return Value();
        }
        if(this->T ==VREAL){
            if(op.T==VREAL){
                return Value(this->GetReal()* op.GetReal());
            }else{
                return Value(this->GetReal() * (double)op.GetInt());
            }
        }else if(this->T==VINT){
            if(op.T==VINT){
                return Value(this->GetInt()*op.GetInt());
            }else{
                return Value((double)this->GetInt()*op.GetReal());
            }
        }
    }
    return Value();
}
Value Value::Catenate(const Value & op)const{
    if(op.IsString() && this->IsString()){
        return Value(this->GetString() + op.GetString());
    }
    return Value();
}
Value Value::Power(const Value & op)const{
    if(this->IsInt() && op.IsInt()){
        return Value(pow((double)this->GetInt(),(double)op.GetInt()));
    }
    if(this->IsReal() && op.IsReal()){
        return Value(pow(this->GetReal(),op.GetReal()));
    }
    if(this->IsInt() && op.IsReal()){
        return Value(pow((double)this->GetInt(),op.GetReal()));
    }
    if(this->IsReal() && op.IsInt()){
        return Value(pow(this->GetReal(),op.GetInt()));
    }
    return Value();
}
Value Value::operator<(const Value& op)const{
    
    if(this->IsReal() || this->IsInt()){
        if(op.T != VREAL && op.T !=VINT){
            return Value();
        }
        if(this->T ==VREAL){
            if(op.T==VREAL){
                return Value(this->GetReal()< op.GetReal());
            }else{
                return Value(this->GetReal() < (double)op.GetInt());
            }
        }else if(this->T==VINT){
            if(op.T==VINT){
                return Value(this->GetInt()<op.GetInt());
            }else{
                return Value((double)this->GetInt()<op.GetReal());
            }
        }
    }
    return Value();
}
Value Value::operator>(const Value& op)const{
    
    if(this->IsReal() || this->IsInt()){
        if(op.T != VREAL && op.T !=VINT){
            return Value();
        }
        if(this->T ==VREAL){
            if(op.T==VREAL){
                return Value(this->GetReal()> op.GetReal());
            }else{
                return Value(this->GetReal() > (double)op.GetInt());
            }
        }else if(this->T==VINT){
            if(op.T==VINT){
                return Value(this->GetInt()>op.GetInt());
            }else{
                return Value((double)this->GetInt()>op.GetReal());
            }
        }
    }
    return Value();
}
Value Value::operator==(const Value& op)const{
    
    if(this->IsReal() || this->IsInt()){
        if(op.T != VREAL && op.T !=VINT){
            return Value();
        }
        if(this->T ==VREAL){
            if(op.T==VREAL){
                return Value(this->GetReal()== op.GetReal());
            }else{
                return Value(this->GetReal() == (double)op.GetInt());
            }
        }else if(this->T==VINT){
            if(op.T==VINT){
                return Value(this->GetInt()==op.GetInt());
            }else{
                return Value((double)this->GetInt()==op.GetReal());
            }
        }
    }
    return Value();
}
Value Value::operator/(const Value& op)const{
    
    if(this->IsReal() || this->IsInt()){
        if(op.T != VREAL && op.T !=VINT){
            return Value();
        }
        if(this->T ==VREAL){
            if(op.T==VREAL){
                return Value(this->GetReal()/ op.GetReal());
            }else{
                return Value(this->GetReal() / (double)op.GetInt());
            }
        }else if(this->T==VINT){
            if(op.T==VINT){
                return Value(this->GetInt()/op.GetInt());
            }else{
                return Value((double)this->GetInt()/op.GetReal());
            }
        }
    }
    return Value();
}
Value Value::operator+(const Value& op)const{
    
    if(this->IsReal() || this->IsInt()){
        if(op.T != VREAL && op.T !=VINT){
            return Value();
        }
        if(this->T ==VREAL){
            if(op.T==VREAL){
                return Value(this->GetReal() + op.GetReal());
            }else{
                return Value(this->GetReal() + (double)op.GetInt());
            }
        }else if(this->T==VINT){
            if(op.T==VINT){
                return Value(this->GetInt()+op.GetInt());
            }else{
                return Value((double)this->GetInt()+op.GetReal());
            }
        }
    }
    return Value();
}
Value Value::operator-(const Value& op)const{
    
    if(this->IsReal() || this->IsInt()){
        if(op.T != VREAL && op.T !=VINT){
            return Value();
        }
        if(this->T ==VREAL){
            if(op.T==VREAL){
                return Value(this->GetReal()- op.GetReal());
            }else{
                return Value(this->GetReal() - (double)op.GetInt());
            }
        }else if(this->T==VINT){
            if(op.T==VINT){
                return Value(this->GetInt()-op.GetInt());
            }else{
                return Value((double)this->GetInt()-op.GetReal());
            }
        }
    }
    return Value();
}