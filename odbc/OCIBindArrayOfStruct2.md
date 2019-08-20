Hi all,

I'm testing passing in an array of struct and getting one output param from
a pl/sql block in this way:
// code snippet

text *ins_stmt1 = (text *)"begin INSERT INTO ecare_passcode VALUES (:1, :2);
:3 := :1; end;";

typedef struct {
char buf[30];
char buf1[30];
char buf2[60];
} test;

test data1[MAX_IN_ROWS];

ub4 valsk1 = (ub4) sizeof(test); /* value skip */
ub4 indsk = 0; /* indicator skips */
ub4 rlsk = 0; /* return length skips */
ub4 rcsk = 0; /* return code skips */

memset((void *)data1, 0, sizeof(test) * MAX_IN_ROWS);

for (i=0; i<MAX_IN_ROWS; i++)
{
sprintf(data1.buf, "99999%d%d", n, i);
sprintf(data1.buf1, "test%d", i);
}

checkerr(ctxptr->errhp, OCIStmtPrepare(ctxptr->stmthp, ctxptr->errhp,
ins_stmt1, (ub4) strlen((char *)ins_stmt1),
(ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT));


checkerr(ctxptr->errhp, OCIBindByName(ctxptr->stmthp, &bndp1,
ctxptr->errhp, (text *)":1", strlen(":1"), (dvoid *) &data1[0].buf,
(sb4) strlen(data1[0].buf), SQLT_CHR,
(dvoid *) 0, (ub2 *)0, (ub2 *)0,
(ub4) 0, (ub4 *) 0, (ub4) OCI_DEFAULT));

checkerr(ctxptr->errhp, OCIBindByName(ctxptr->stmthp, &bndp2,
ctxptr->errhp, (text *)":2", strlen(":2"), (dvoid *) &data1[0].buf1,
(sb4) strlen(data1[0].buf1), SQLT_CHR,
(dvoid *) 0, (ub2 *)0, (ub2 *)0,
(ub4) 0, (ub4 *) 0, (ub4) OCI_DEFAULT));

checkerr(ctxptr->errhp, OCIBindByName(ctxptr->stmthp, &bndp3,
ctxptr->errhp, (text *)":3", strlen(":3"), (dvoid *) data1[0].buf2,
(sb4)60, SQLT_STR,
(dvoid *) 0, (ub2 *)0, (ub2 *)0,
(ub4) 0, (ub4 *) 0, (ub4) OCI_DEFAULT));


if (OCIBindArrayOfStruct(bndp1,ctxptr->errhp,valsk1, indsk, rlsk, rcsk)
|| OCIBindArrayOfStruct(bndp2,ctxptr->errhp,valsk1, indsk, rlsk, rcsk)
|| OCIBindArrayOfStruct(bndp3,ctxptr->errhp,valsk1, indsk, rlsk, rcsk))
return;

checkerr(ctxptr->errhp, OCIStmtExecute(ctxptr->svchp, ctxptr->stmthp,
ctxptr->errhp, (ub4) MAX_IN_ROWS, (ub4)0,
(OCISnapshot *) NULL, (OCISnapshot *) NULL,
OCI_DEFAULT));


for(i = 0; i < MAX_IN_ROWS; i++) {
printf("i = %d, len = %d, %s******\n", i, strlen(data1.buf2), data1.buf2);
}
//end code snip

And here is what the output:
i = 0, len = 7, 9999900******
i = 1, len = 7, 9999902******
i = 2, len = 7, 9999904******
i = 3, len = 7, 9999906******
i = 4, len = 7, 9999908******
i = 5, len = 0, ******
i = 6, len = 0, ******
i = 7, len = 0, ******
i = 8, len = 0, ******
i = 9, len = 0, ******

also tried several other combination of the value of out param's size,
OCIBindByPos(), OCIBindArrayOfStruct(), and.., none of them seems work. Is
it possible make it work? If not, any suggestions?


TIA
