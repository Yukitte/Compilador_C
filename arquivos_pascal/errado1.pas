program VariavelNaoDeclarada;
var
    a, b: integer;
begin
    a := 5;
    b := 10;
    soma := a + b;  { Erro: 'soma' não foi declarada }
    writeln('A soma é: ', soma);
end.
