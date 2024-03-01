#include "shell.h"
#include <stdio.h>

/**
 * check_chain_delimiter - test if current char in buffer is a chain delimiter
 * @info: the parameter struct
 * @buf: the char buffer
 * @p: address of current position in buf
 *
 * Return: 1 if chain delimiter, 0 otherwise
 */
int check_chain_delimiter(info_t *info, char *buf, size_t *p)
{
    size_t j = *p;

    if (buf[j] == '|' && buf[j + 1] == '|')
    {
        buf[j] = 0;
        j++;
        info->cmd_buf_type = 1;
    }
    else if (buf[j] == '&' && buf[j + 1] == '&')
    {
        buf[j] = 0;
        j++;
        info->cmd_buf_type = 2;
    }
    else if (buf[j] == ';') /* found end of this command */
    {
        buf[j] = 0; /* replace semicolon with null */
        info->cmd_buf_type = 3;
    }
    else
        return (0);
    *p = j;
    return (1);
}

/**
 * check_continue_chaining - checks if we should continue chaining based on last status
 * @info: the parameter struct
 * @buf: the char buffer
 * @p: address of current position in buf
 * @i: starting position in buf
 * @len: length of buf
 *
 * Return: Void
 */
void check_continue_chaining(info_t *info, char *buf, size_t *p, size_t i, size_t len)
{
    size_t j = *p;

    if (info->cmd_buf_type == 2)
    {
        if (info->status)
        {
            buf[i] = 0;
            j = len;
        }
    }
    if (info->cmd_buf_type == 1)
    {
        if (!info->status)
        {
            buf[i] = 0;
            j = len;
        }
    }

    *p = j;
}

/**
 * replace_aliases - replaces aliases in the tokenized string
 * @info: the parameter struct
 *
 * Return: 1 if replaced, 0 otherwise
 */
int replace_aliases(info_t *info)
{
    int i;
    list_t *node;
    char *p;

    for (i = 0; i < 10; i++)
    {
        node = find_alias(info->alias, info->argv[0], '=');
        if (!node)
            return (0);
        free(info->argv[0]);
        p = find_separator(node->str, '=');
        if (!p)
            return (0);
        p = trim(p + 1);
        if (!p)
            return (0);
        info->argv[0] = p;
    }
    return (1);
}

/**
 * replace_vars - replaces variables in the tokenized string
 * @info: the parameter struct
 *
 * Return: 1 if replaced, 0 otherwise
 */
int replace_vars(info_t *info)
{
    int i = 0;
    list_t *node;

    for (i = 0; info->argv[i]; i++)
    {
        if (info->argv[i][0] != '$' || !info->argv[i][1])
            continue;

        if (!check_for_var(info->argv[i], "$?"))
        {
            replace_string(&(info->argv[i]),
                           trim(int_to_str(info->status)));
            continue;
        }
        if (!check_for_var(info->argv[i], "$$"))
        {
            replace_string(&(info->argv[i]),
                           trim(int_to_str(getpid())));
            continue;
        }
        node = find_variable(info->env, &info->argv[i][1], '=');
        if (node)
        {
            replace_string(&(info->argv[i]),
                           trim(find_separator(node->str, '=') + 1));
            continue;
        }
        replace_string(&info->argv[i], trim(""));
    }
    return (0);
}

/**
 * replace_string - replaces string
 * @old: address of old string
 * @new: new string
 *
 * Return: 1 if replaced, 0 otherwise
 */
int replace_string(char **old, char *new)
{
    free(*old);
    *old = strdup(new);
    return (1);
}
