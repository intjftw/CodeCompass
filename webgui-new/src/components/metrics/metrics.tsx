import {
  Button,
  Checkbox,
  FormControl,
  InputLabel,
  ListItemText,
  MenuItem,
  OutlinedInput,
  Select,
  Tooltip,
} from '@mui/material';
import { FileName } from 'components/file-name/file-name';
import React, { Dispatch, SetStateAction, useContext, useEffect, useMemo, useState } from 'react';
import { getMetrics, getMetricsTypeNames } from 'service/metrics-service';
import { FileInfo, MetricsType, MetricsTypeName } from '@thrift-generated';
import { Treemap } from 'recharts';
import { AppContext } from 'global-context/app-context';
import { getFileInfoByPath } from 'service/project-service';
import * as SC from './styled-components';
import { useTranslation } from 'react-i18next';

type RespType = {
  [key: string]: {
    [key: string]: string | RespType;
  };
};

type DataItem = {
  name: string;
  size: number;
  children?: DataItem[];
};

type CustomTreeNodeProps = {
  root?: CustomTreeNodeProps;
  index?: number;
  x?: number;
  y?: number;
  width?: number;
  height?: number;
  depth?: number;
  size?: number;
  name?: string;
  children?: CustomTreeNodeProps[];
};

const CustomizedContent = (
  props: CustomTreeNodeProps & {
    setData: Dispatch<SetStateAction<DataItem[] | undefined>>;
    filePath: string;
    setFilePath: Dispatch<SetStateAction<string>>;
    setFileInfo: Dispatch<SetStateAction<FileInfo | undefined>>;
  }
): JSX.Element => {
  const { x, y, width, height, depth, size, name, children } = props as {
    x: number;
    y: number;
    width: number;
    height: number;
    depth: number;
    size: number;
    name: string;
    children: CustomTreeNodeProps[];
  };

  const getDarkenedRGBCode = (value: number): string => {
    const normalizedValue = Math.min(Math.max(value, 0), 255);
    const red = Math.max(Math.round((1 - normalizedValue / 255) * 80), 10);
    const green = Math.max(Math.round((1 - normalizedValue / 255) * 120), 10);
    const blue = Math.max(Math.round((1 - normalizedValue / 255) * 255), 80);
    const rgbCode = `rgb(${red}, ${green}, ${blue})`;
    return rgbCode;
  };

  const updateTreemap = async () => {
    if (!children) return;

    const newPath = `${props.filePath}/${name}`;
    const newFileInfo = await getFileInfoByPath(newPath);
    const newData = children.map((child) =>
      Object.fromEntries([
        ['name', child.name],
        ['size', child.size],
        ['children', child.children],
      ])
    );

    props.setFileInfo(newFileInfo);
    props.setFilePath(newPath);
    props.setData(newData);
  };

  return depth === 1 ? (
    <Tooltip title={size}>
      <g onClick={() => updateTreemap()}>
        <SC.StyledRect
          x={x}
          y={y}
          width={width}
          height={height}
          sx={{
            fill: getDarkenedRGBCode(size),
            stroke: '#FFF',
            ':hover': {
              fill: props.children ? '#9597E4' : getDarkenedRGBCode(size),
            },
            cursor: props.children ? 'pointer' : '',
          }}
        />
        <text x={x + width / 2} y={y + height / 2} textAnchor="middle" fill="#fff" fontSize={14} strokeWidth={0.1}>
          {name}
        </text>
      </g>
    </Tooltip>
  ) : (
    <></>
  );
};

const fileTypeOptions = ['Unknown', 'Dir', 'Binary', 'CPP'];

const sumSizes = (node: DataItem): number => {
  let sum = 0;
  if (!node.children) {
    sum += node.size ?? 0;
  } else {
    node.children.forEach((child) => (sum += sumSizes(child)));
  }
  return sum;
};

const findChildren = (node: DataItem): DataItem[] | undefined => {
  if (node.children && node.children.length > 1) {
    return node.children;
  } else if (node.children && node.children.length === 1) {
    return findChildren(node.children[0]);
  } else {
    return undefined;
  }
};

const convertResObject = (res: RespType): DataItem => {
  const keys = Object.keys(res);
  const name = keys[0];
  const obj = res[name];
  const children = [];

  for (const key in obj) {
    const value = obj[key];

    if (typeof value === 'object') {
      const child = convertResObject({ [key]: value } as RespType);
      child.name = key;
      child.size = sumSizes(child);
      children.push(child);
    } else {
      children.push({ name: key, size: Number(value) });
    }
  }

  return { name, size: 0, children };
};

export const Metrics = (): JSX.Element => {
  const { t } = useTranslation();
  const appCtx = useContext(AppContext);

  const [fileInfo, setFileInfo] = useState<FileInfo | undefined>(undefined);
  const [filePath, setFilePath] = useState<string>('');
  const [initialPath, setInitialPath] = useState<string>('');
  const [data, setData] = useState<DataItem[] | undefined>(undefined);
  const [selectedFileTypeOptions, setSelectedFileTypeOptions] = useState<string[]>(fileTypeOptions);
  const [sizeDimension, setSizeDimension] = useState<MetricsTypeName | undefined>(undefined);
  const [metricsTypeNames, setMetricsTypeNames] = useState<MetricsTypeName[]>([]);

  useEffect(() => {
    if (!appCtx.metricsGenId) return;
    const init = async () => {
      const initMetricsTypeNames = await getMetricsTypeNames();

      const metricsRes = await getMetrics(
        appCtx.metricsGenId,
        fileTypeOptions,
        initMetricsTypeNames[0].type as MetricsType
      );
      const convertedObject = convertResObject(JSON.parse(metricsRes));

      let path = `/${convertedObject.name}`;
      let item = convertedObject;
      while (item.children?.length === 1) {
        item = item.children[0];
        path += `/${item.name}`;
      }

      const initFileInfo = await getFileInfoByPath(path);

      setMetricsTypeNames(initMetricsTypeNames);
      setSizeDimension(initMetricsTypeNames[0]);
      setFileInfo(initFileInfo);
      setFilePath(path);
      setInitialPath(path);
      setData(findChildren(convertedObject));
    };
    init();
  }, [appCtx.metricsGenId]);

  const generateMetrics = async (fPath?: string) => {
    const fInfo = fPath ? await getFileInfoByPath(fPath) : fileInfo;
    const metricsRes = await getMetrics(
      fInfo?.id as string,
      selectedFileTypeOptions,
      sizeDimension?.type as MetricsType
    );
    const convertedObject = convertResObject(JSON.parse(metricsRes));
    if (fPath) {
      setFileInfo(fInfo);
      setFilePath(fPath);
    }
    setData(findChildren(convertedObject));
  };

  const renderTreeMap = useMemo(() => {
    return data && filePath ? (
      <Treemap
        width={900}
        height={900}
        data={data}
        dataKey="size"
        stroke="#fff"
        fill="#8884d8"
        content={
          <CustomizedContent
            setData={setData}
            filePath={filePath}
            setFilePath={setFilePath}
            setFileInfo={setFileInfo}
          />
        }
        isAnimationActive={false}
      />
    ) : (
      <></>
    );
  }, [data, filePath]);

  return appCtx.metricsGenId && fileInfo && sizeDimension ? (
    <>
      <FileName
        fileName={fileInfo ? (fileInfo.name as string) : ''}
        filePath={fileInfo ? (fileInfo.path as string) : ''}
        parseStatus={fileInfo ? (fileInfo.parseStatus as number) : 4}
        info={fileInfo ?? undefined}
      />
      <SC.OuterContainer>
        <SC.MetricsOptionsContainer>
          <FormControl sx={{ width: 300 }}>
            <InputLabel>{t('metrics.fileType')}</InputLabel>
            <Select
              multiple
              value={selectedFileTypeOptions}
              onChange={(e) => {
                const {
                  target: { value },
                } = e;
                setSelectedFileTypeOptions(typeof value === 'string' ? value.split(',') : value);
              }}
              input={<OutlinedInput label={t('metrics.fileType')} />}
              renderValue={(selected) => selected.join(', ')}
            >
              {fileTypeOptions.map((type) => (
                <MenuItem key={type} value={type}>
                  <Checkbox checked={selectedFileTypeOptions.indexOf(type) > -1} />
                  <ListItemText primary={type} />
                </MenuItem>
              ))}
            </Select>
          </FormControl>
          <FormControl>
            <InputLabel>{t('metrics.sizeDimension')}</InputLabel>
            <Select
              value={sizeDimension.name}
              label={t('metrics.sizeDimension')}
              onChange={(e) =>
                setSizeDimension(
                  metricsTypeNames.find((typeName) => typeName.name === e.target.value) as MetricsTypeName
                )
              }
            >
              {metricsTypeNames.map((option, idx) => (
                <MenuItem key={idx} value={option.name}>
                  {option.name}
                </MenuItem>
              ))}
            </Select>
          </FormControl>
          <Button onClick={() => generateMetrics()}>{t('metrics.drawMetrics')}</Button>
        </SC.MetricsOptionsContainer>
        <SC.StyledBreadcrumbs>
          {filePath.split('/').map((p, idx) => (
            <SC.StyledDiv
              key={idx}
              sx={
                idx !== filePath.split('/').length - 1 && !initialPath.slice(0, -1).includes(p)
                  ? {
                      cursor: 'pointer',
                      ':hover': {
                        color: (theme) => theme.backgroundColors?.secondary,
                      },
                    }
                  : {}
              }
              onClick={() => {
                if (idx === filePath.split('/').length - 1 || initialPath.slice(0, -1).includes(p)) return;
                const trimmedPath = filePath
                  ?.split('/')
                  .slice(0, idx + 1)
                  .join('/') as string;
                generateMetrics(trimmedPath);
              }}
            >
              <div>{p}</div>
            </SC.StyledDiv>
          ))}
        </SC.StyledBreadcrumbs>
        <SC.MetricsContainer>{renderTreeMap}</SC.MetricsContainer>
      </SC.OuterContainer>
    </>
  ) : (
    <SC.Placeholder>{t('metrics.noMetrics')}</SC.Placeholder>
  );
};
