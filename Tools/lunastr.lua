local lstr = {}

-- check whether the input object is serialzable
lstr.serializable = function(o)
    local t = type(o)
    if  t == "number" or t == "boolean" or t == "nil" or t == "string" then
        return true
    elseif t == "table" then
        local is_serializable = true
        for k, v in pairs(o) do
            is_serializable = is_serializable and lstr.serializable(k) and lstr.serializable(v)
            if not is_serializable then
                break
            end
        end
        return is_serializable
    else
        return false
    end
end

lstr.serialize = function(o, indent)
    indent = indent or 0

    if not lstr.serializable(o) then
        error("The input object is contains types that are not serializable.")
    end

    local make_indent = function(indent)
        if indent ~= 0 then
            local s = ""
            for i = 1, indent do
                s = s .. "    "
            end
            return s
        else
            return ""
        end
    end

    local t = type(o)
    local s = ""
    if t == "number" or t == "boolean" or t == "nil" then
        s = s .. string.format("%q", o)
    elseif t == "string" then
        s = s .. "\"" .. o .. "\""
    elseif t == "table" then
        if next(o) == nil then
            s = s .. "{}"
        else
            s = s .. "{\n"
            for k, v in pairs(o) do
                s = s .. make_indent(indent + 1)
                local tk = type(k)
                if tk == "string" then
                    s = s .. k .. " = "
                else
                    s = s .. "[" .. k .. "] = "
                end
                s = s .. lstr.serialize(v, indent + 1)
                s = s .. ",\n"
            end
            s = s .. make_indent(indent) .. "}"
        end
    end
    return s
end

lstr.dump = function(o, indent)
    indent = indent or 0

    local make_indent = function(indent)
        if indent ~= 0 then
            local s = ""
            for i = 1, indent do
                s = s .. "    "
            end
            return s
        else
            return ""
        end
    end

    local t = type(o)
    local s = ""
    if t == "number" or t == "boolean" or t == "nil" then
        s = s .. string.format("%q", o)
    elseif t == "string" then
        s = s .. "\"" .. o .. "\""
    elseif t == "table" then
        if next(o) == nil then
            s = s .. "{}"
        else
            s = s .. "{\n"
            for k, v in pairs(o) do
                s = s .. make_indent(indent + 1)
                local tk = type(k)
                if tk == "string" then
                    s = s .. k .. " = "
                elseif tk == "number" or tk == "boolean" or tk == "nil" then
                    s = s .. "[" .. k .. "] = "
                else
                    s = s .. "[[" .. tk .. "]] = " -- display type.
                end
                s = s .. lstr.dump(v, indent + 1)
                s = s .. ",\n"
            end
            s = s .. make_indent(indent) .. "}"
        end
    else 
        s = s .. "[[" .. t .. "]]" -- display type.
    end
    return s
end

-- split the string to multiple sub-strings based on specific slice characters.
-- for example: "I love banana" -> {"I", "love", "banana"} (based on whitespace).
lstr.split = function(str, slice_str)
    local last = 1
    local slice_len = #slice_str
    local sep = string.find(str, slice_str)
    if not sep then return str end
    local r = {}
    while sep do
        local sub_str = string.sub(str, last, sep - 1)
        if sub_str ~= "" then r[#r + 1] = sub_str end 
        last = sep + slice_len
        sep = string.find(str, slice_str, last)
    end
    local sub_str = string.sub(str, last, -1)
    if sub_str ~= "" then r[#r + 1] = sub_str end
    return r
end

return lstr