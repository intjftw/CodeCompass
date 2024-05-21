import {
  Checkbox,
  FormControl,
  FormControlLabel,
  FormGroup,
  FormLabel,
  IconButton,
  InputLabel,
  MenuItem,
  Radio,
  RadioGroup,
  Select,
  Tooltip,
} from '@mui/material';
import React, { Dispatch, SetStateAction, useEffect, useState } from 'react';
import { SearchMainLanguages, SearchOptions, SearchOtherLanguages, SearchTypeOptions } from 'enums/search-enum';
import { enumToArray, removeFromArray } from 'utils/utils';
import { Info, Close } from '@mui/icons-material';
import { getStore, setStore } from 'utils/store';
import { SearchType } from '@thrift-generated';
import * as SC from './styled-components';
import { useTranslation } from 'react-i18next';

export const SettingsMenu = ({
  anchorEl,
  setAnchorEl,
  searchTypes,
  searchType,
  setSearchType,
  searchLanguage,
  setSearchLanguage,
  selectedSearchTypeOptions,
  setSelectedSearchTypeOptions,
}: {
  anchorEl: null | HTMLElement;
  setAnchorEl: Dispatch<SetStateAction<HTMLElement | null>>;
  searchTypes: SearchType[];
  searchType: SearchType;
  setSearchType: Dispatch<SetStateAction<SearchType | undefined>>;
  searchLanguage: string;
  setSearchLanguage: Dispatch<SetStateAction<string | undefined>>;
  selectedSearchTypeOptions: string[];
  setSelectedSearchTypeOptions: Dispatch<SetStateAction<string[] | undefined>>;
}): JSX.Element => {
  const { t } = useTranslation();

  const searchMainLanguages = enumToArray(SearchMainLanguages);
  const searchOtherLanguages = enumToArray(SearchOtherLanguages) as string[];
  const searchTypeOptions = enumToArray(SearchTypeOptions);

  const [searchOtherLanguage, setSearchOtherLanguage] = useState<string | undefined>(undefined);
  const [searchLanguagesDisabled, setSearchLanguagesDisabled] = useState<boolean>(
    searchType?.name !== SearchOptions.TEXT.toString() || searchType?.name !== SearchOptions.DEFINITION.toString()
  );
  const [searchTypeOptionsDisabled, setSearchTypeOptionsDisabled] = useState<boolean>(
    searchType?.name !== SearchOptions.DEFINITION.toString()
  );

  useEffect(() => {
    const initSearchOtherLanguage = enumToArray(SearchOtherLanguages)[0];
    const { storedSearchOtherLanguage } = getStore();
    setSearchOtherLanguage(storedSearchOtherLanguage ?? initSearchOtherLanguage);
  }, []);

  useEffect(() => {
    setSearchLanguagesDisabled(
      searchType?.name !== SearchOptions.TEXT.toString() && searchType?.name !== SearchOptions.DEFINITION.toString()
    );
    setSearchTypeOptionsDisabled(searchType?.name !== SearchOptions.DEFINITION.toString());
  }, [searchType]);

  useEffect(() => {
    setStore({
      storedSearchOtherLanguage: searchOtherLanguage,
    });
  }, [searchOtherLanguage]);

  const Options = () => {
    return (
      <div>
        <FormLabel>{t('searchSettings.menu.searchOptions')}</FormLabel>
        <RadioGroup value={searchType?.name ?? searchTypes[0].name}>
          {searchTypes.map((elem, idx) => {
            return (
              <FormControlLabel
                key={idx}
                onClick={() => setSearchType(elem)}
                value={elem.name}
                control={<Radio />}
                label={elem.name}
              />
            );
          })}
        </RadioGroup>
      </div>
    );
  };

  const Languages = () => {
    return (
      <div>
        <FormLabel>{t('searchSettings.menu.languages')}</FormLabel>
        <RadioGroup value={searchLanguage ?? searchMainLanguages[0]}>
          {searchMainLanguages.map((elem, idx) => {
            return (
              <FormControlLabel
                disabled={searchLanguagesDisabled}
                key={idx}
                onClick={() => {
                  if (searchLanguagesDisabled) return;
                  setSearchLanguage(elem);
                }}
                value={elem}
                control={<Radio />}
                label={elem}
              />
            );
          })}
          <SC.RadioWithInfo>
            <FormControlLabel
              disabled={searchLanguagesDisabled}
              onClick={() => {
                if (!searchLanguagesDisabled) {
                  setSearchLanguage('Any');
                }
              }}
              value={'Any'}
              control={<Radio />}
              label={t('searchSettings.menu.any')}
            />
            <Tooltip title={t('searchSettings.tooltips.anyTooltip')}>
              <Info />
            </Tooltip>
          </SC.RadioWithInfo>
          <SC.OtherLanguagesContainer>
            <FormControlLabel
              disabled={searchLanguagesDisabled}
              onClick={() => {
                if (!searchLanguagesDisabled) {
                  setSearchLanguage(searchOtherLanguage as string);
                }
              }}
              value={''}
              control={<Radio />}
              label={''}
              checked={searchOtherLanguages.includes(searchLanguage)}
            />
            <FormControl>
              <InputLabel>{t('searchSettings.menu.other')}</InputLabel>
              <Select
                disabled={searchLanguagesDisabled}
                value={searchOtherLanguage}
                label={t('searchSettings.menu.other')}
                onChange={(e) => {
                  if (!searchLanguagesDisabled) {
                    setSearchOtherLanguage(e.target.value);

                    if (searchOtherLanguages.includes(searchLanguage)) {
                      setSearchLanguage(e.target.value);
                    }
                  }
                }}
                MenuProps={{
                  PaperProps: {
                    style: {
                      maxHeight: 200,
                      width: 200,
                      marginTop: '5px',
                      border: '1px solid white',
                    },
                  },
                }}
              >
                {searchOtherLanguages.map((elem, idx) => {
                  return (
                    <MenuItem key={idx} value={elem}>
                      {elem}
                    </MenuItem>
                  );
                })}
              </Select>
            </FormControl>
          </SC.OtherLanguagesContainer>
        </RadioGroup>
      </div>
    );
  };

  const Types = () => {
    return (
      <FormGroup>
        <FormLabel>{t('searchSettings.menu.types')}</FormLabel>
        {searchTypeOptions.map((elem, idx) => {
          return (
            <FormControlLabel
              disabled={searchTypeOptionsDisabled}
              key={idx}
              control={
                <Checkbox
                  onChange={(e) =>
                    !searchTypeOptionsDisabled
                      ? setSelectedSearchTypeOptions(
                          e.currentTarget.checked
                            ? [...selectedSearchTypeOptions, elem]
                            : removeFromArray(selectedSearchTypeOptions, elem)
                        )
                      : ''
                  }
                  checked={selectedSearchTypeOptions.includes(elem)}
                />
              }
              label={elem}
            />
          );
        })}
        <FormControlLabel
          disabled={searchTypeOptionsDisabled}
          control={
            <Checkbox
              onChange={(e) =>
                !searchTypeOptionsDisabled
                  ? setSelectedSearchTypeOptions(e.currentTarget.checked ? searchTypeOptions : [])
                  : ''
              }
              checked={searchTypeOptions.every((t) => selectedSearchTypeOptions.includes(t))}
            />
          }
          label={t('searchSettings.menu.all')}
        />
      </FormGroup>
    );
  };

  return (
    <SC.StyledMenu anchorEl={anchorEl} open={Boolean(anchorEl)} onClose={() => setAnchorEl(null)}>
      <SC.Container>
        <FormControl>
          <SC.ExpressionSearchSettings>
            <Options />
            <Languages />
            <Types />
          </SC.ExpressionSearchSettings>
        </FormControl>
        <IconButton onClick={() => setAnchorEl(null)}>
          <Close />
        </IconButton>
      </SC.Container>
    </SC.StyledMenu>
  );
};
