@for /D %%v in (*) do @msgfmt -c -o %%v\messages.mo %%v\messages.po 
