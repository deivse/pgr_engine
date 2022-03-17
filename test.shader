shader::fragment {
    #version 400
    uniform float test;
    out float y;

    void main(){
        y = test;
    }
} shader::fragment

shader::vertex {
    #version 400

    void main(){

    }
} shader::vertex