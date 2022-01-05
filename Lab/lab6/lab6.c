void Set_Xaddr(char x);
void Set_Yaddr(char y);
void Send_Data(char bitmap);

void Draw (char x, char y, char bitmap)
{
    Set_Xaddr(x);
    Set_Yaddr(y);
    Send_Data(bitmap);
}

void Set_Xaddr(char x)
{
    char P2_cword, P4_cword;

    //prepare control words
    P2_cword = P2_CWORD_TEMPLATE;
    P2_cword = p2_cword & (~P2_RS); //clear RS bit
    P2_cword = P2_cword & (~P2_RW); //clear RW bit
    P4_cword = P4_Set_Xaddr_TMPL;
    P4_cword = P4_cword | (x & 0x07);

    //flush out control signals
    while(GLCD_IsBusy());
    GLCD_Write(P2_cword, P4_cword);
}

void Set_Yaddr(char y)
{
    char P2_cword, P4_cword;

    //prepare control words
    P2_cword = P2_CWORD_TEMPLATE;
    P2_cword = P2_cword & (~P2_RS); //clear RS bit
    P2_cword = P2_cword & (~P2_RW); //clear RW bit
    P4_cword = P4_Set_Yaddr_TMPL;
    P4_cword = P4_cword | (y & 0x3f);

    //flush out control signals
    while (GLCD_IsBusy());
    GLCD_Write(P2_cword, P4_cword);
}//end of function Set_Yaddr

void Send_Data (char pattern)
{
    char P2_cword, P4_cword;

    //prepare control words
    P2_cword = P2_CWORD_TEMPLATE;
    P2_cword = P2_cword | (P2_RS);  //set RS bit
    P2_cword = P2_cword & (~P2_RW); //clear RW bit
    P4_cword = pattern;

    //flush out control signals
    while(GLCD_IsBusy());
    GLCD_Write(P2_cword, P4_cword);
}//end of function Send_Data